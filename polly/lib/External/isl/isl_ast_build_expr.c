/*
 * Copyright 2012-2014 Ecole Normale Superieure
 *
 * Use of this software is governed by the MIT license
 *
 * Written by Sven Verdoolaege,
 * Ecole Normale Superieure, 45 rue d’Ulm, 75230 Paris, France
 */

#include <isl/ilp.h>
#include <isl_ast_build_expr.h>
#include <isl_ast_private.h>
#include <isl_ast_build_private.h>

/* Compute the "opposite" of the (numerator of the) argument of a div
 * with denonimator "d".
 *
 * In particular, compute
 *
 *	-aff + (d - 1)
 */
static __isl_give isl_aff *oppose_div_arg(__isl_take isl_aff *aff,
	__isl_take isl_val *d)
{
	aff = isl_aff_neg(aff);
	aff = isl_aff_add_constant_val(aff, d);
	aff = isl_aff_add_constant_si(aff, -1);

	return aff;
}

/* Create an isl_ast_expr evaluating the div at position "pos" in "ls".
 * The result is simplified in terms of build->domain.
 *
 * *change_sign is set by this function if the sign of
 * the expression has changed.
 * "ls" is known to be non-NULL.
 *
 * Let the div be of the form floor(e/d).
 * If the ast_build_prefer_pdiv option is set then we check if "e"
 * is non-negative, so that we can generate
 *
 *	(pdiv_q, expr(e), expr(d))
 *
 * instead of
 *
 *	(fdiv_q, expr(e), expr(d))
 *
 * If the ast_build_prefer_pdiv option is set and
 * if "e" is not non-negative, then we check if "-e + d - 1" is non-negative.
 * If so, we can rewrite
 *
 *	floor(e/d) = -ceil(-e/d) = -floor((-e + d - 1)/d)
 *
 * and still use pdiv_q.
 */
static __isl_give isl_ast_expr *var_div(int *change_sign,
	__isl_keep isl_local_space *ls,
	int pos, __isl_keep isl_ast_build *build)
{
	isl_ctx *ctx = isl_local_space_get_ctx(ls);
	isl_aff *aff;
	isl_ast_expr *num, *den;
	isl_val *d;
	enum isl_ast_op_type type;

	aff = isl_local_space_get_div(ls, pos);
	d = isl_aff_get_denominator_val(aff);
	aff = isl_aff_scale_val(aff, isl_val_copy(d));
	den = isl_ast_expr_from_val(isl_val_copy(d));

	type = isl_ast_op_fdiv_q;
	if (isl_options_get_ast_build_prefer_pdiv(ctx)) {
		int non_neg = isl_ast_build_aff_is_nonneg(build, aff);
		if (non_neg >= 0 && !non_neg) {
			isl_aff *opp = oppose_div_arg(isl_aff_copy(aff),
							isl_val_copy(d));
			non_neg = isl_ast_build_aff_is_nonneg(build, opp);
			if (non_neg >= 0 && non_neg) {
				*change_sign = 1;
				isl_aff_free(aff);
				aff = opp;
			} else
				isl_aff_free(opp);
		}
		if (non_neg < 0)
			aff = isl_aff_free(aff);
		else if (non_neg)
			type = isl_ast_op_pdiv_q;
	}

	isl_val_free(d);
	num = isl_ast_expr_from_aff(aff, build);
	return isl_ast_expr_alloc_binary(type, num, den);
}

/* Create an isl_ast_expr evaluating the specified dimension of "ls".
 * The result is simplified in terms of build->domain.
 *
 * *change_sign is set by this function if the sign of
 * the expression has changed.
 *
 * The isl_ast_expr is constructed based on the type of the dimension.
 * - divs are constructed by var_div
 * - set variables are constructed from the iterator isl_ids in "build"
 * - parameters are constructed from the isl_ids in "ls"
 */
static __isl_give isl_ast_expr *var(int *change_sign,
	__isl_keep isl_local_space *ls,
	enum isl_dim_type type, int pos, __isl_keep isl_ast_build *build)
{
	isl_ctx *ctx = isl_local_space_get_ctx(ls);
	isl_id *id;

	if (type == isl_dim_div)
		return var_div(change_sign, ls, pos, build);

	if (type == isl_dim_set) {
		id = isl_ast_build_get_iterator_id(build, pos);
		return isl_ast_expr_from_id(id);
	}

	if (!isl_local_space_has_dim_id(ls, type, pos))
		isl_die(ctx, isl_error_internal, "unnamed dimension",
			return NULL);
	id = isl_local_space_get_dim_id(ls, type, pos);
	return isl_ast_expr_from_id(id);
}

/* Does "expr" represent the zero integer?
 */
static int ast_expr_is_zero(__isl_keep isl_ast_expr *expr)
{
	if (!expr)
		return -1;
	if (expr->type != isl_ast_expr_int)
		return 0;
	return isl_val_is_zero(expr->u.v);
}

/* Create an expression representing the sum of "expr1" and "expr2",
 * provided neither of the two expressions is identically zero.
 */
static __isl_give isl_ast_expr *ast_expr_add(__isl_take isl_ast_expr *expr1,
	__isl_take isl_ast_expr *expr2)
{
	if (!expr1 || !expr2)
		goto error;

	if (ast_expr_is_zero(expr1)) {
		isl_ast_expr_free(expr1);
		return expr2;
	}

	if (ast_expr_is_zero(expr2)) {
		isl_ast_expr_free(expr2);
		return expr1;
	}

	return isl_ast_expr_add(expr1, expr2);
error:
	isl_ast_expr_free(expr1);
	isl_ast_expr_free(expr2);
	return NULL;
}

/* Subtract expr2 from expr1.
 *
 * If expr2 is zero, we simply return expr1.
 * If expr1 is zero, we return
 *
 *	(isl_ast_op_minus, expr2)
 *
 * Otherwise, we return
 *
 *	(isl_ast_op_sub, expr1, expr2)
 */
static __isl_give isl_ast_expr *ast_expr_sub(__isl_take isl_ast_expr *expr1,
	__isl_take isl_ast_expr *expr2)
{
	if (!expr1 || !expr2)
		goto error;

	if (ast_expr_is_zero(expr2)) {
		isl_ast_expr_free(expr2);
		return expr1;
	}

	if (ast_expr_is_zero(expr1)) {
		isl_ast_expr_free(expr1);
		return isl_ast_expr_neg(expr2);
	}

	return isl_ast_expr_sub(expr1, expr2);
error:
	isl_ast_expr_free(expr1);
	isl_ast_expr_free(expr2);
	return NULL;
}

/* Return an isl_ast_expr that represents
 *
 *	v * (aff mod d)
 *
 * v is assumed to be non-negative.
 * The result is simplified in terms of build->domain.
 */
static __isl_give isl_ast_expr *isl_ast_expr_mod(__isl_keep isl_val *v,
	__isl_keep isl_aff *aff, __isl_keep isl_val *d,
	__isl_keep isl_ast_build *build)
{
	isl_ctx *ctx;
	isl_ast_expr *expr;
	isl_ast_expr *c;

	if (!aff)
		return NULL;

	ctx = isl_aff_get_ctx(aff);
	expr = isl_ast_expr_from_aff(isl_aff_copy(aff), build);

	c = isl_ast_expr_from_val(isl_val_copy(d));
	expr = isl_ast_expr_alloc_binary(isl_ast_op_pdiv_r, expr, c);

	if (!isl_val_is_one(v)) {
		c = isl_ast_expr_from_val(isl_val_copy(v));
		expr = isl_ast_expr_mul(c, expr);
	}

	return expr;
}

/* Create an isl_ast_expr that scales "expr" by "v".
 *
 * If v is 1, we simply return expr.
 * If v is -1, we return
 *
 *	(isl_ast_op_minus, expr)
 *
 * Otherwise, we return
 *
 *	(isl_ast_op_mul, expr(v), expr)
 */
static __isl_give isl_ast_expr *scale(__isl_take isl_ast_expr *expr,
	__isl_take isl_val *v)
{
	isl_ast_expr *c;

	if (!expr || !v)
		goto error;
	if (isl_val_is_one(v)) {
		isl_val_free(v);
		return expr;
	}

	if (isl_val_is_negone(v)) {
		isl_val_free(v);
		expr = isl_ast_expr_neg(expr);
	} else {
		c = isl_ast_expr_from_val(v);
		expr = isl_ast_expr_mul(c, expr);
	}

	return expr;
error:
	isl_val_free(v);
	isl_ast_expr_free(expr);
	return NULL;
}

/* Add an expression for "*v" times the specified dimension of "ls"
 * to expr.
 *
 * Let e be the expression for the specified dimension,
 * multiplied by the absolute value of "*v".
 * If "*v" is negative, we create
 *
 *	(isl_ast_op_sub, expr, e)
 *
 * except when expr is trivially zero, in which case we create
 *
 *	(isl_ast_op_minus, e)
 *
 * instead.
 *
 * If "*v" is positive, we simply create
 *
 *	(isl_ast_op_add, expr, e)
 *
 */
static __isl_give isl_ast_expr *isl_ast_expr_add_term(
	__isl_take isl_ast_expr *expr,
	__isl_keep isl_local_space *ls, enum isl_dim_type type, int pos,
	__isl_take isl_val *v, __isl_keep isl_ast_build *build)
{
	isl_ast_expr *term;
	int change_sign;

	if (!expr)
		return NULL;

	change_sign = 0;
	term = var(&change_sign, ls, type, pos, build);
	if (change_sign)
		v = isl_val_neg(v);

	if (isl_val_is_neg(v) && !ast_expr_is_zero(expr)) {
		v = isl_val_neg(v);
		term = scale(term, v);
		return ast_expr_sub(expr, term);
	} else {
		term = scale(term, v);
		return ast_expr_add(expr, term);
	}
}

/* Add an expression for "v" to expr.
 */
static __isl_give isl_ast_expr *isl_ast_expr_add_int(
	__isl_take isl_ast_expr *expr, __isl_take isl_val *v)
{
	isl_ctx *ctx;
	isl_ast_expr *expr_int;

	if (!expr || !v)
		goto error;

	if (isl_val_is_zero(v)) {
		isl_val_free(v);
		return expr;
	}

	ctx = isl_ast_expr_get_ctx(expr);
	if (isl_val_is_neg(v) && !ast_expr_is_zero(expr)) {
		v = isl_val_neg(v);
		expr_int = isl_ast_expr_from_val(v);
		return ast_expr_sub(expr, expr_int);
	} else {
		expr_int = isl_ast_expr_from_val(v);
		return ast_expr_add(expr, expr_int);
	}
error:
	isl_ast_expr_free(expr);
	isl_val_free(v);
	return NULL;
}

/* Internal data structure used inside extract_modulos.
 *
 * If any modulo expressions are detected in "aff", then the
 * expression is removed from "aff" and added to either "pos" or "neg"
 * depending on the sign of the coefficient of the modulo expression
 * inside "aff".
 *
 * "add" is an expression that needs to be added to "aff" at the end of
 * the computation.  It is NULL as long as no modulos have been extracted.
 *
 * "i" is the position in "aff" of the div under investigation
 * "v" is the coefficient in "aff" of the div
 * "div" is the argument of the div, with the denominator removed
 * "d" is the original denominator of the argument of the div
 *
 * "nonneg" is an affine expression that is non-negative over "build"
 * and that can be used to extract a modulo expression from "div".
 * In particular, if "sign" is 1, then the coefficients of "nonneg"
 * are equal to those of "div" modulo "d".  If "sign" is -1, then
 * the coefficients of "nonneg" are opposite to those of "div" modulo "d".
 * If "sign" is 0, then no such affine expression has been found (yet).
 */
struct isl_extract_mod_data {
	isl_ast_build *build;
	isl_aff *aff;

	isl_ast_expr *pos;
	isl_ast_expr *neg;

	isl_aff *add;

	int i;
	isl_val *v;
	isl_val *d;
	isl_aff *div;

	isl_aff *nonneg;
	int sign;
};

/* Given that data->v * div_i in data->aff is equal to
 *
 *	f * (term - (arg mod d))
 *
 * with data->d * f = data->v, add
 *
 *	f * term
 *
 * to data->add and
 *
 *	abs(f) * (arg mod d)
 *
 * to data->neg or data->pos depending on the sign of -f.
 */
static int extract_term_and_mod(struct isl_extract_mod_data *data,
	__isl_take isl_aff *term, __isl_take isl_aff *arg)
{
	isl_ast_expr *expr;
	int s;

	data->v = isl_val_div(data->v, isl_val_copy(data->d));
	s = isl_val_sgn(data->v);
	data->v = isl_val_abs(data->v);
	expr = isl_ast_expr_mod(data->v, arg, data->d, data->build);
	isl_aff_free(arg);
	if (s > 0)
		data->neg = ast_expr_add(data->neg, expr);
	else
		data->pos = ast_expr_add(data->pos, expr);
	data->aff = isl_aff_set_coefficient_si(data->aff,
						isl_dim_div, data->i, 0);
	if (s < 0)
		data->v = isl_val_neg(data->v);
	term = isl_aff_scale_val(data->div, isl_val_copy(data->v));

	if (!data->add)
		data->add = term;
	else
		data->add = isl_aff_add(data->add, term);
	if (!data->add)
		return -1;

	return 0;
}

/* Given that data->v * div_i in data->aff is of the form
 *
 *	f * d * floor(div/d)
 *
 * with div nonnegative on data->build, rewrite it as
 *
 *	f * (div - (div mod d)) = f * div - f * (div mod d)
 *
 * and add
 *
 *	f * div
 *
 * to data->add and
 *
 *	abs(f) * (div mod d)
 *
 * to data->neg or data->pos depending on the sign of -f.
 */
static int extract_mod(struct isl_extract_mod_data *data)
{
	return extract_term_and_mod(data, isl_aff_copy(data->div),
			isl_aff_copy(data->div));
}

/* Given that data->v * div_i in data->aff is of the form
 *
 *	f * d * floor(div/d)					(1)
 *
 * check if div is non-negative on data->build and, if so,
 * extract the corresponding modulo from data->aff.
 * If not, then check if
 *
 *	-div + d - 1
 *
 * is non-negative on data->build.  If so, replace (1) by
 *
 *	-f * d * floor((-div + d - 1)/d)
 *
 * and extract the corresponding modulo from data->aff.
 *
 * This function may modify data->div.
 */
static int extract_nonneg_mod(struct isl_extract_mod_data *data)
{
	int mod;

	mod = isl_ast_build_aff_is_nonneg(data->build, data->div);
	if (mod < 0)
		goto error;
	if (mod)
		return extract_mod(data);

	data->div = oppose_div_arg(data->div, isl_val_copy(data->d));
	mod = isl_ast_build_aff_is_nonneg(data->build, data->div);
	if (mod < 0)
		goto error;
	if (mod) {
		data->v = isl_val_neg(data->v);
		return extract_mod(data);
	}

	return 0;
error:
	data->aff = isl_aff_free(data->aff);
	return -1;
}

/* Is the affine expression of constraint "c" "simpler" than data->nonneg
 * for use in extracting a modulo expression?
 *
 * We currently only consider the constant term of the affine expression.
 * In particular, we prefer the affine expression with the smallest constant
 * term.
 * This means that if there are two constraints, say x >= 0 and -x + 10 >= 0,
 * then we would pick x >= 0
 *
 * More detailed heuristics could be used if it turns out that there is a need.
 */
static int mod_constraint_is_simpler(struct isl_extract_mod_data *data,
	__isl_keep isl_constraint *c)
{
	isl_val *v1, *v2;
	int simpler;

	if (!data->nonneg)
		return 1;

	v1 = isl_val_abs(isl_constraint_get_constant_val(c));
	v2 = isl_val_abs(isl_aff_get_constant_val(data->nonneg));
	simpler = isl_val_lt(v1, v2);
	isl_val_free(v1);
	isl_val_free(v2);

	return simpler;
}

/* Check if the coefficients of "c" are either equal or opposite to those
 * of data->div modulo data->d.  If so, and if "c" is "simpler" than
 * data->nonneg, then replace data->nonneg by the affine expression of "c"
 * and set data->sign accordingly.
 *
 * Both "c" and data->div are assumed not to involve any integer divisions.
 *
 * Before we start the actual comparison, we first quickly check if
 * "c" and data->div have the same non-zero coefficients.
 * If not, then we assume that "c" is not of the desired form.
 * Note that while the coefficients of data->div can be reasonably expected
 * not to involve any coefficients that are multiples of d, "c" may
 * very well involve such coefficients.  This means that we may actually
 * miss some cases.
 */
static int check_parallel_or_opposite(__isl_take isl_constraint *c, void *user)
{
	struct isl_extract_mod_data *data = user;
	enum isl_dim_type c_type[2] = { isl_dim_param, isl_dim_set };
	enum isl_dim_type a_type[2] = { isl_dim_param, isl_dim_in };
	int i, t;
	int n[2];
	int parallel = 1, opposite = 1;

	for (t = 0; t < 2; ++t) {
		n[t] = isl_constraint_dim(c, c_type[t]);
		for (i = 0; i < n[t]; ++i) {
			int a, b;

			a = isl_constraint_involves_dims(c, c_type[t], i, 1);
			b = isl_aff_involves_dims(data->div, a_type[t], i, 1);
			if (a != b)
				parallel = opposite = 0;
		}
	}

	for (t = 0; t < 2; ++t) {
		for (i = 0; i < n[t]; ++i) {
			isl_val *v1, *v2;

			if (!parallel && !opposite)
				break;
			v1 = isl_constraint_get_coefficient_val(c,
								c_type[t], i);
			v2 = isl_aff_get_coefficient_val(data->div,
								a_type[t], i);
			if (parallel) {
				v1 = isl_val_sub(v1, isl_val_copy(v2));
				parallel = isl_val_is_divisible_by(v1, data->d);
				v1 = isl_val_add(v1, isl_val_copy(v2));
			}
			if (opposite) {
				v1 = isl_val_add(v1, isl_val_copy(v2));
				opposite = isl_val_is_divisible_by(v1, data->d);
			}
			isl_val_free(v1);
			isl_val_free(v2);
		}
	}

	if ((parallel || opposite) && mod_constraint_is_simpler(data, c)) {
		isl_aff_free(data->nonneg);
		data->nonneg = isl_constraint_get_aff(c);
		data->sign = parallel ? 1 : -1;
	}

	isl_constraint_free(c);

	if (data->sign != 0 && data->nonneg == NULL)
		return -1;

	return 0;
}

/* Given that data->v * div_i in data->aff is of the form
 *
 *	f * d * floor(div/d)					(1)
 *
 * see if we can find an expression div' that is non-negative over data->build
 * and that is related to div through
 *
 *	div' = div + d * e
 *
 * or
 *
 *	div' = -div + d - 1 + d * e
 *
 * with e some affine expression.
 * If so, we write (1) as
 *
 *	f * div + f * (div' mod d)
 *
 * or
 *
 *	-f * (-div + d - 1) - f * (div' mod d)
 *
 * exploiting (in the second case) the fact that
 *
 *	f * d * floor(div/d) =	-f * d * floor((-div + d - 1)/d)
 *
 *
 * We first try to find an appropriate expression for div'
 * from the constraints of data->build->domain (which is therefore
 * guaranteed to be non-negative on data->build), where we remove
 * any integer divisions from the constraints and skip this step
 * if "div" itself involves any integer divisions.
 * If we cannot find an appropriate expression this way, then
 * we pass control to extract_nonneg_mod where check
 * if div or "-div + d -1" themselves happen to be
 * non-negative on data->build.
 *
 * While looking for an appropriate constraint in data->build->domain,
 * we ignore the constant term, so after finding such a constraint,
 * we still need to fix up the constant term.
 * In particular, if a is the constant term of "div"
 * (or d - 1 - the constant term of "div" if data->sign < 0)
 * and b is the constant term of the constraint, then we need to find
 * a non-negative constant c such that
 *
 *	b + c \equiv a	mod d
 *
 * We therefore take
 *
 *	c = (a - b) mod d
 *
 * and add it to b to obtain the constant term of div'.
 * If this constant term is "too negative", then we add an appropriate
 * multiple of d to make it positive.
 *
 *
 * Note that the above is a only a very simple heuristic for finding an
 * appropriate expression.  We could try a bit harder by also considering
 * sums of constraints that involve disjoint sets of variables or
 * we could consider arbitrary linear combinations of constraints,
 * although that could potentially be much more expensive as it involves
 * the solution of an LP problem.
 *
 * In particular, if v_i is a column vector representing constraint i,
 * w represents div and e_i is the i-th unit vector, then we are looking
 * for a solution of the constraints
 *
 *	\sum_i lambda_i v_i = w + \sum_i alpha_i d e_i
 *
 * with \lambda_i >= 0 and alpha_i of unrestricted sign.
 * If we are not just interested in a non-negative expression, but
 * also in one with a minimal range, then we don't just want
 * c = \sum_i lambda_i v_i to be non-negative over the domain,
 * but also beta - c = \sum_i mu_i v_i, where beta is a scalar
 * that we want to minimize and we now also have to take into account
 * the constant terms of the constraints.
 * Alternatively, we could first compute the dual of the domain
 * and plug in the constraints on the coefficients.
 */
static int try_extract_mod(struct isl_extract_mod_data *data)
{
	isl_basic_set *hull;
	isl_val *v1, *v2;
	int r;

	if (!data->build)
		goto error;

	int n = isl_aff_dim(data->div, isl_dim_div);

	if (isl_aff_involves_dims(data->div, isl_dim_div, 0, n))
		return extract_nonneg_mod(data);

	hull = isl_set_simple_hull(isl_set_copy(data->build->domain));
	hull = isl_basic_set_remove_divs(hull);
	data->sign = 0;
	data->nonneg = NULL;
	r = isl_basic_set_foreach_constraint(hull, &check_parallel_or_opposite,
					data);
	isl_basic_set_free(hull);

	if (!data->sign || r < 0) {
		isl_aff_free(data->nonneg);
		if (r < 0)
			goto error;
		return extract_nonneg_mod(data);
	}

	v1 = isl_aff_get_constant_val(data->div);
	v2 = isl_aff_get_constant_val(data->nonneg);
	if (data->sign < 0) {
		v1 = isl_val_neg(v1);
		v1 = isl_val_add(v1, isl_val_copy(data->d));
		v1 = isl_val_sub_ui(v1, 1);
	}
	v1 = isl_val_sub(v1, isl_val_copy(v2));
	v1 = isl_val_mod(v1, isl_val_copy(data->d));
	v1 = isl_val_add(v1, v2);
	v2 = isl_val_div(isl_val_copy(v1), isl_val_copy(data->d));
	v2 = isl_val_ceil(v2);
	if (isl_val_is_neg(v2)) {
		v2 = isl_val_mul(v2, isl_val_copy(data->d));
		v1 = isl_val_sub(v1, isl_val_copy(v2));
	}
	data->nonneg = isl_aff_set_constant_val(data->nonneg, v1);
	isl_val_free(v2);

	if (data->sign < 0) {
		data->div = oppose_div_arg(data->div, isl_val_copy(data->d));
		data->v = isl_val_neg(data->v);
	}

	return extract_term_and_mod(data,
				    isl_aff_copy(data->div), data->nonneg);
error:
	data->aff = isl_aff_free(data->aff);
	return -1;
}

/* Check if "data->aff" involves any (implicit) modulo computations based
 * on div "data->i".
 * If so, remove them from aff and add expressions corresponding
 * to those modulo computations to data->pos and/or data->neg.
 *
 * "aff" is assumed to be an integer affine expression.
 *
 * In particular, check if (v * div_j) is of the form
 *
 *	f * m * floor(a / m)
 *
 * and, if so, rewrite it as
 *
 *	f * (a - (a mod m)) = f * a - f * (a mod m)
 *
 * and extract out -f * (a mod m).
 * In particular, if f > 0, we add (f * (a mod m)) to *neg.
 * If f < 0, we add ((-f) * (a mod m)) to *pos.
 *
 * Note that in order to represent "a mod m" as
 *
 *	(isl_ast_op_pdiv_r, a, m)
 *
 * we need to make sure that a is non-negative.
 * If not, we check if "-a + m - 1" is non-negative.
 * If so, we can rewrite
 *
 *	floor(a/m) = -ceil(-a/m) = -floor((-a + m - 1)/m)
 *
 * and still extract a modulo.
 */
static int extract_modulo(struct isl_extract_mod_data *data)
{
	data->div = isl_aff_get_div(data->aff, data->i);
	data->d = isl_aff_get_denominator_val(data->div);
	if (isl_val_is_divisible_by(data->v, data->d)) {
		data->div = isl_aff_scale_val(data->div, isl_val_copy(data->d));
		if (try_extract_mod(data) < 0)
			data->aff = isl_aff_free(data->aff);
	}
	isl_aff_free(data->div);
	isl_val_free(data->d);
	return 0;
}

/* Check if "aff" involves any (implicit) modulo computations.
 * If so, remove them from aff and add expressions corresponding
 * to those modulo computations to *pos and/or *neg.
 * We only do this if the option ast_build_prefer_pdiv is set.
 *
 * "aff" is assumed to be an integer affine expression.
 *
 * A modulo expression is of the form
 *
 *	a mod m = a - m * floor(a / m)
 *
 * To detect them in aff, we look for terms of the form
 *
 *	f * m * floor(a / m)
 *
 * rewrite them as
 *
 *	f * (a - (a mod m)) = f * a - f * (a mod m)
 *
 * and extract out -f * (a mod m).
 * In particular, if f > 0, we add (f * (a mod m)) to *neg.
 * If f < 0, we add ((-f) * (a mod m)) to *pos.
 */
static __isl_give isl_aff *extract_modulos(__isl_take isl_aff *aff,
	__isl_keep isl_ast_expr **pos, __isl_keep isl_ast_expr **neg,
	__isl_keep isl_ast_build *build)
{
	struct isl_extract_mod_data data = { build, aff, *pos, *neg };
	isl_ctx *ctx;
	int n;

	if (!aff)
		return NULL;

	ctx = isl_aff_get_ctx(aff);
	if (!isl_options_get_ast_build_prefer_pdiv(ctx))
		return aff;

	n = isl_aff_dim(data.aff, isl_dim_div);
	for (data.i = 0; data.i < n; ++data.i) {
		data.v = isl_aff_get_coefficient_val(data.aff,
							isl_dim_div, data.i);
		if (!data.v)
			return isl_aff_free(aff);
		if (isl_val_is_zero(data.v) ||
		    isl_val_is_one(data.v) || isl_val_is_negone(data.v)) {
			isl_val_free(data.v);
			continue;
		}
		if (extract_modulo(&data) < 0)
			data.aff = isl_aff_free(data.aff);
		isl_val_free(data.v);
		if (!data.aff)
			break;
	}

	if (data.add)
		data.aff = isl_aff_add(data.aff, data.add);

	*pos = data.pos;
	*neg = data.neg;
	return data.aff;
}

/* Check if aff involves any non-integer coefficients.
 * If so, split aff into
 *
 *	aff = aff1 + (aff2 / d)
 *
 * with both aff1 and aff2 having only integer coefficients.
 * Return aff1 and add (aff2 / d) to *expr.
 */
static __isl_give isl_aff *extract_rational(__isl_take isl_aff *aff,
	__isl_keep isl_ast_expr **expr, __isl_keep isl_ast_build *build)
{
	int i, j, n;
	isl_aff *rat = NULL;
	isl_local_space *ls = NULL;
	isl_ast_expr *rat_expr;
	isl_val *v, *d;
	enum isl_dim_type t[] = { isl_dim_param, isl_dim_in, isl_dim_div };
	enum isl_dim_type l[] = { isl_dim_param, isl_dim_set, isl_dim_div };

	if (!aff)
		return NULL;
	d = isl_aff_get_denominator_val(aff);
	if (!d)
		goto error;
	if (isl_val_is_one(d)) {
		isl_val_free(d);
		return aff;
	}

	aff = isl_aff_scale_val(aff, isl_val_copy(d));

	ls = isl_aff_get_domain_local_space(aff);
	rat = isl_aff_zero_on_domain(isl_local_space_copy(ls));

	for (i = 0; i < 3; ++i) {
		n = isl_aff_dim(aff, t[i]);
		for (j = 0; j < n; ++j) {
			isl_aff *rat_j;

			v = isl_aff_get_coefficient_val(aff, t[i], j);
			if (!v)
				goto error;
			if (isl_val_is_divisible_by(v, d)) {
				isl_val_free(v);
				continue;
			}
			rat_j = isl_aff_var_on_domain(isl_local_space_copy(ls),
							l[i], j);
			rat_j = isl_aff_scale_val(rat_j, v);
			rat = isl_aff_add(rat, rat_j);
		}
	}

	v = isl_aff_get_constant_val(aff);
	if (isl_val_is_divisible_by(v, d)) {
		isl_val_free(v);
	} else {
		isl_aff *rat_0;

		rat_0 = isl_aff_val_on_domain(isl_local_space_copy(ls), v);
		rat = isl_aff_add(rat, rat_0);
	}

	isl_local_space_free(ls);

	aff = isl_aff_sub(aff, isl_aff_copy(rat));
	aff = isl_aff_scale_down_val(aff, isl_val_copy(d));

	rat_expr = isl_ast_expr_from_aff(rat, build);
	rat_expr = isl_ast_expr_div(rat_expr, isl_ast_expr_from_val(d));
	*expr = ast_expr_add(*expr, rat_expr);

	return aff;
error:
	isl_aff_free(rat);
	isl_local_space_free(ls);
	isl_aff_free(aff);
	isl_val_free(d);
	return NULL;
}

/* Construct an isl_ast_expr that evaluates the affine expression "aff",
 * The result is simplified in terms of build->domain.
 *
 * We first extract hidden modulo computations from the affine expression
 * and then add terms for each variable with a non-zero coefficient.
 * Finally, if the affine expression has a non-trivial denominator,
 * we divide the resulting isl_ast_expr by this denominator.
 */
__isl_give isl_ast_expr *isl_ast_expr_from_aff(__isl_take isl_aff *aff,
	__isl_keep isl_ast_build *build)
{
	int i, j;
	int n;
	isl_val *v;
	isl_ctx *ctx = isl_aff_get_ctx(aff);
	isl_ast_expr *expr, *expr_neg;
	enum isl_dim_type t[] = { isl_dim_param, isl_dim_in, isl_dim_div };
	enum isl_dim_type l[] = { isl_dim_param, isl_dim_set, isl_dim_div };
	isl_local_space *ls;

	if (!aff)
		return NULL;

	expr = isl_ast_expr_alloc_int_si(ctx, 0);
	expr_neg = isl_ast_expr_alloc_int_si(ctx, 0);

	aff = extract_rational(aff, &expr, build);

	aff = extract_modulos(aff, &expr, &expr_neg, build);
	expr = ast_expr_sub(expr, expr_neg);

	ls = isl_aff_get_domain_local_space(aff);

	for (i = 0; i < 3; ++i) {
		n = isl_aff_dim(aff, t[i]);
		for (j = 0; j < n; ++j) {
			v = isl_aff_get_coefficient_val(aff, t[i], j);
			if (!v)
				expr = isl_ast_expr_free(expr);
			if (isl_val_is_zero(v)) {
				isl_val_free(v);
				continue;
			}
			expr = isl_ast_expr_add_term(expr,
							ls, l[i], j, v, build);
		}
	}

	v = isl_aff_get_constant_val(aff);
	expr = isl_ast_expr_add_int(expr, v);

	isl_local_space_free(ls);
	isl_aff_free(aff);
	return expr;
}

/* Add terms to "expr" for each variable in "aff" with a coefficient
 * with sign equal to "sign".
 * The result is simplified in terms of build->domain.
 */
static __isl_give isl_ast_expr *add_signed_terms(__isl_take isl_ast_expr *expr,
	__isl_keep isl_aff *aff, int sign, __isl_keep isl_ast_build *build)
{
	int i, j;
	isl_val *v;
	enum isl_dim_type t[] = { isl_dim_param, isl_dim_in, isl_dim_div };
	enum isl_dim_type l[] = { isl_dim_param, isl_dim_set, isl_dim_div };
	isl_local_space *ls;

	ls = isl_aff_get_domain_local_space(aff);

	for (i = 0; i < 3; ++i) {
		int n = isl_aff_dim(aff, t[i]);
		for (j = 0; j < n; ++j) {
			v = isl_aff_get_coefficient_val(aff, t[i], j);
			if (sign * isl_val_sgn(v) <= 0) {
				isl_val_free(v);
				continue;
			}
			v = isl_val_abs(v);
			expr = isl_ast_expr_add_term(expr,
						ls, l[i], j, v, build);
		}
	}

	isl_local_space_free(ls);

	return expr;
}

/* Should the constant term "v" be considered positive?
 *
 * A positive constant will be added to "pos" by the caller,
 * while a negative constant will be added to "neg".
 * If either "pos" or "neg" is exactly zero, then we prefer
 * to add the constant "v" to that side, irrespective of the sign of "v".
 * This results in slightly shorter expressions and may reduce the risk
 * of overflows.
 */
static int constant_is_considered_positive(__isl_keep isl_val *v,
	__isl_keep isl_ast_expr *pos, __isl_keep isl_ast_expr *neg)
{
	if (ast_expr_is_zero(pos))
		return 1;
	if (ast_expr_is_zero(neg))
		return 0;
	return isl_val_is_pos(v);
}

/* Check if the equality
 *
 *	aff = 0
 *
 * represents a stride constraint on the integer division "pos".
 *
 * In particular, if the integer division "pos" is equal to
 *
 *	floor(e/d)
 *
 * then check if aff is equal to
 *
 *	e - d floor(e/d)
 *
 * or its opposite.
 *
 * If so, the equality is exactly
 *
 *	e mod d = 0
 *
 * Note that in principle we could also accept
 *
 *	e - d floor(e'/d)
 *
 * where e and e' differ by a constant.
 */
static int is_stride_constraint(__isl_keep isl_aff *aff, int pos)
{
	isl_aff *div;
	isl_val *c, *d;
	int eq;

	div = isl_aff_get_div(aff, pos);
	c = isl_aff_get_coefficient_val(aff, isl_dim_div, pos);
	d = isl_aff_get_denominator_val(div);
	eq = isl_val_abs_eq(c, d);
	if (eq >= 0 && eq) {
		aff = isl_aff_copy(aff);
		aff = isl_aff_set_coefficient_si(aff, isl_dim_div, pos, 0);
		div = isl_aff_scale_val(div, d);
		if (isl_val_is_pos(c))
			div = isl_aff_neg(div);
		eq = isl_aff_plain_is_equal(div, aff);
		isl_aff_free(aff);
	} else
		isl_val_free(d);
	isl_val_free(c);
	isl_aff_free(div);

	return eq;
}

/* Are all coefficients of "aff" (zero or) negative?
 */
static int all_negative_coefficients(__isl_keep isl_aff *aff)
{
	int i, n;

	if (!aff)
		return 0;

	n = isl_aff_dim(aff, isl_dim_param);
	for (i = 0; i < n; ++i)
		if (isl_aff_coefficient_sgn(aff, isl_dim_param, i) > 0)
			return 0;

	n = isl_aff_dim(aff, isl_dim_in);
	for (i = 0; i < n; ++i)
		if (isl_aff_coefficient_sgn(aff, isl_dim_in, i) > 0)
			return 0;

	return 1;
}

/* Give an equality of the form
 *
 *	aff = e - d floor(e/d) = 0
 *
 * or
 *
 *	aff = -e + d floor(e/d) = 0
 *
 * with the integer division "pos" equal to floor(e/d),
 * construct the AST expression
 *
 *	(isl_ast_op_eq, (isl_ast_op_zdiv_r, expr(e), expr(d)), expr(0))
 *
 * If e only has negative coefficients, then construct
 *
 *	(isl_ast_op_eq, (isl_ast_op_zdiv_r, expr(-e), expr(d)), expr(0))
 *
 * instead.
 */
static __isl_give isl_ast_expr *extract_stride_constraint(
	__isl_take isl_aff *aff, int pos, __isl_keep isl_ast_build *build)
{
	isl_ctx *ctx;
	isl_val *c;
	isl_ast_expr *expr, *cst;

	if (!aff)
		return NULL;

	ctx = isl_aff_get_ctx(aff);

	c = isl_aff_get_coefficient_val(aff, isl_dim_div, pos);
	aff = isl_aff_set_coefficient_si(aff, isl_dim_div, pos, 0);

	if (all_negative_coefficients(aff))
		aff = isl_aff_neg(aff);

	cst = isl_ast_expr_from_val(isl_val_abs(c));
	expr = isl_ast_expr_from_aff(aff, build);

	expr = isl_ast_expr_alloc_binary(isl_ast_op_zdiv_r, expr, cst);
	cst = isl_ast_expr_alloc_int_si(ctx, 0);
	expr = isl_ast_expr_alloc_binary(isl_ast_op_eq, expr, cst);

	return expr;
}

/* Construct an isl_ast_expr that evaluates the condition "constraint",
 * The result is simplified in terms of build->domain.
 *
 * We first check if the constraint is an equality of the form
 *
 *	e - d floor(e/d) = 0
 *
 * i.e.,
 *
 *	e mod d = 0
 *
 * If so, we convert it to
 *
 *	(isl_ast_op_eq, (isl_ast_op_zdiv_r, expr(e), expr(d)), expr(0))
 *
 * Otherwise, let the constraint by either "a >= 0" or "a == 0".
 * We first extract hidden modulo computations from "a"
 * and then collect all the terms with a positive coefficient in cons_pos
 * and the terms with a negative coefficient in cons_neg.
 *
 * The result is then of the form
 *
 *	(isl_ast_op_ge, expr(pos), expr(-neg)))
 *
 * or
 *
 *	(isl_ast_op_eq, expr(pos), expr(-neg)))
 *
 * However, if the first expression is an integer constant (and the second
 * is not), then we swap the two expressions.  This ensures that we construct,
 * e.g., "i <= 5" rather than "5 >= i".
 *
 * Furthermore, is there are no terms with positive coefficients (or no terms
 * with negative coefficients), then the constant term is added to "pos"
 * (or "neg"), ignoring the sign of the constant term.
 */
static __isl_give isl_ast_expr *isl_ast_expr_from_constraint(
	__isl_take isl_constraint *constraint, __isl_keep isl_ast_build *build)
{
	int i, n;
	isl_ctx *ctx;
	isl_ast_expr *expr_pos;
	isl_ast_expr *expr_neg;
	isl_ast_expr *expr;
	isl_aff *aff;
	isl_val *v;
	int eq;
	enum isl_ast_op_type type;

	if (!constraint)
		return NULL;

	aff = isl_constraint_get_aff(constraint);
	eq = isl_constraint_is_equality(constraint);
	isl_constraint_free(constraint);

	n = isl_aff_dim(aff, isl_dim_div);
	if (eq && n > 0)
		for (i = 0; i < n; ++i) {
			int is_stride;
			is_stride = is_stride_constraint(aff, i);
			if (is_stride < 0)
				goto error;
			if (is_stride)
				return extract_stride_constraint(aff, i, build);
		}

	ctx = isl_aff_get_ctx(aff);
	expr_pos = isl_ast_expr_alloc_int_si(ctx, 0);
	expr_neg = isl_ast_expr_alloc_int_si(ctx, 0);

	aff = extract_modulos(aff, &expr_pos, &expr_neg, build);

	expr_pos = add_signed_terms(expr_pos, aff, 1, build);
	expr_neg = add_signed_terms(expr_neg, aff, -1, build);

	v = isl_aff_get_constant_val(aff);
	if (constant_is_considered_positive(v, expr_pos, expr_neg)) {
		expr_pos = isl_ast_expr_add_int(expr_pos, v);
	} else {
		v = isl_val_neg(v);
		expr_neg = isl_ast_expr_add_int(expr_neg, v);
	}

	if (isl_ast_expr_get_type(expr_pos) == isl_ast_expr_int &&
	    isl_ast_expr_get_type(expr_neg) != isl_ast_expr_int) {
		type = eq ? isl_ast_op_eq : isl_ast_op_le;
		expr = isl_ast_expr_alloc_binary(type, expr_neg, expr_pos);
	} else {
		type = eq ? isl_ast_op_eq : isl_ast_op_ge;
		expr = isl_ast_expr_alloc_binary(type, expr_pos, expr_neg);
	}

	isl_aff_free(aff);
	return expr;
error:
	isl_aff_free(aff);
	return NULL;
}

/* Wrapper around isl_constraint_cmp_last_non_zero for use
 * as a callback to isl_constraint_list_sort.
 * If isl_constraint_cmp_last_non_zero cannot tell the constraints
 * apart, then use isl_constraint_plain_cmp instead.
 */
static int cmp_constraint(__isl_keep isl_constraint *a,
	__isl_keep isl_constraint *b, void *user)
{
	int cmp;

	cmp = isl_constraint_cmp_last_non_zero(a, b);
	if (cmp != 0)
		return cmp;
	return isl_constraint_plain_cmp(a, b);
}

/* Construct an isl_ast_expr that evaluates the conditions defining "bset".
 * The result is simplified in terms of build->domain.
 *
 * If "bset" is not bounded by any constraint, then we contruct
 * the expression "1", i.e., "true".
 *
 * Otherwise, we sort the constraints, putting constraints that involve
 * integer divisions after those that do not, and construct an "and"
 * of the ast expressions of the individual constraints.
 *
 * Each constraint is added to the generated constraints of the build
 * after it has been converted to an AST expression so that it can be used
 * to simplify the following constraints.  This may change the truth value
 * of subsequent constraints that do not satisfy the earlier constraints,
 * but this does not affect the outcome of the conjunction as it is
 * only true if all the conjuncts are true (no matter in what order
 * they are evaluated).  In particular, the constraints that do not
 * involve integer divisions may serve to simplify some constraints
 * that do involve integer divisions.
 */
__isl_give isl_ast_expr *isl_ast_build_expr_from_basic_set(
	 __isl_keep isl_ast_build *build, __isl_take isl_basic_set *bset)
{
	int i, n;
	isl_constraint *c;
	isl_constraint_list *list;
	isl_ast_expr *res;
	isl_set *set;

	list = isl_basic_set_get_constraint_list(bset);
	isl_basic_set_free(bset);
	list = isl_constraint_list_sort(list, &cmp_constraint, NULL);
	if (!list)
		return NULL;
	n = isl_constraint_list_n_constraint(list);
	if (n == 0) {
		isl_ctx *ctx = isl_basic_set_get_ctx(bset);
		isl_constraint_list_free(list);
		return isl_ast_expr_alloc_int_si(ctx, 1);
	}

	build = isl_ast_build_copy(build);

	c = isl_constraint_list_get_constraint(list, 0);
	bset = isl_basic_set_from_constraint(isl_constraint_copy(c));
	set = isl_set_from_basic_set(bset);
	res = isl_ast_expr_from_constraint(c, build);
	build = isl_ast_build_restrict_generated(build, set);

	for (i = 1; i < n; ++i) {
		isl_ast_expr *expr;

		c = isl_constraint_list_get_constraint(list, i);
		bset = isl_basic_set_from_constraint(isl_constraint_copy(c));
		set = isl_set_from_basic_set(bset);
		expr = isl_ast_expr_from_constraint(c, build);
		build = isl_ast_build_restrict_generated(build, set);
		res = isl_ast_expr_and(res, expr);
	}

	isl_constraint_list_free(list);
	isl_ast_build_free(build);
	return res;
}

struct isl_expr_from_set_data {
	isl_ast_build *build;
	int first;
	isl_ast_expr *res;
};

/* Construct an isl_ast_expr that evaluates the conditions defining "bset"
 * and add it to data->res.
 * The result is simplified in terms of data->build->domain.
 */
static int expr_from_set(__isl_take isl_basic_set *bset, void *user)
{
	struct isl_expr_from_set_data *data = user;
	isl_ast_expr *expr;

	expr = isl_ast_build_expr_from_basic_set(data->build, bset);
	if (data->first)
		data->res = expr;
	else
		data->res = isl_ast_expr_or(data->res, expr);

	data->first = 0;

	if (!data->res)
		return -1;
	return 0;
}

/* Construct an isl_ast_expr that evaluates the conditions defining "set".
 * The result is simplified in terms of build->domain.
 *
 * If "set" is an (obviously) empty set, then return the expression "0".
 */
__isl_give isl_ast_expr *isl_ast_build_expr_from_set(
	__isl_keep isl_ast_build *build, __isl_take isl_set *set)
{
	struct isl_expr_from_set_data data = { build, 1, NULL };

	if (isl_set_foreach_basic_set(set, &expr_from_set, &data) < 0)
		data.res = isl_ast_expr_free(data.res);
	else if (data.first) {
		isl_ctx *ctx = isl_ast_build_get_ctx(build);
		data.res = isl_ast_expr_from_val(isl_val_zero(ctx));
	}

	isl_set_free(set);
	return data.res;
}

struct isl_from_pw_aff_data {
	isl_ast_build *build;
	int n;
	isl_ast_expr **next;
	isl_set *dom;
};

/* This function is called during the construction of an isl_ast_expr
 * that evaluates an isl_pw_aff.
 * Adjust data->next to take into account this piece.
 *
 * data->n is the number of pairs of set and aff to go.
 * data->dom is the domain of the entire isl_pw_aff.
 *
 * If this is the last pair, then data->next is set to evaluate aff
 * and the domain is ignored.
 * Otherwise, data->next is set to a select operation that selects
 * an isl_ast_expr correponding to "aff" on "set" and to an expression
 * that will be filled in by later calls otherwise.
 *
 * In both cases, the constraints of "set" are added to the generated
 * constraints of the build such that they can be exploited to simplify
 * the AST expression constructed from "aff".
 */
static int ast_expr_from_pw_aff(__isl_take isl_set *set,
	__isl_take isl_aff *aff, void *user)
{
	struct isl_from_pw_aff_data *data = user;
	isl_ctx *ctx;
	isl_ast_build *build;

	ctx = isl_set_get_ctx(set);
	data->n--;
	if (data->n == 0) {
		build = isl_ast_build_copy(data->build);
		build = isl_ast_build_restrict_generated(build, set);
		*data->next = isl_ast_expr_from_aff(aff, build);
		isl_ast_build_free(build);
		if (!*data->next)
			return -1;
	} else {
		isl_ast_expr *ternary, *arg;
		isl_set *gist;

		ternary = isl_ast_expr_alloc_op(ctx, isl_ast_op_select, 3);
		gist = isl_set_gist(isl_set_copy(set), isl_set_copy(data->dom));
		arg = isl_ast_build_expr_from_set(data->build, gist);
		ternary = isl_ast_expr_set_op_arg(ternary, 0, arg);
		build = isl_ast_build_copy(data->build);
		build = isl_ast_build_restrict_generated(build, set);
		arg = isl_ast_expr_from_aff(aff, build);
		isl_ast_build_free(build);
		ternary = isl_ast_expr_set_op_arg(ternary, 1, arg);
		if (!ternary)
			return -1;

		*data->next = ternary;
		data->next = &ternary->u.op.args[2];
	}

	return 0;
}

/* Construct an isl_ast_expr that evaluates "pa".
 * The result is simplified in terms of build->domain.
 *
 * The domain of "pa" lives in the internal schedule space.
 */
__isl_give isl_ast_expr *isl_ast_build_expr_from_pw_aff_internal(
	__isl_keep isl_ast_build *build, __isl_take isl_pw_aff *pa)
{
	struct isl_from_pw_aff_data data;
	isl_ast_expr *res = NULL;

	pa = isl_ast_build_compute_gist_pw_aff(build, pa);
	pa = isl_pw_aff_coalesce(pa);
	if (!pa)
		return NULL;

	data.build = build;
	data.n = isl_pw_aff_n_piece(pa);
	data.next = &res;
	data.dom = isl_pw_aff_domain(isl_pw_aff_copy(pa));

	if (isl_pw_aff_foreach_piece(pa, &ast_expr_from_pw_aff, &data) < 0)
		res = isl_ast_expr_free(res);
	else if (!res)
		isl_die(isl_pw_aff_get_ctx(pa), isl_error_invalid,
			"cannot handle void expression", res = NULL);

	isl_pw_aff_free(pa);
	isl_set_free(data.dom);
	return res;
}

/* Construct an isl_ast_expr that evaluates "pa".
 * The result is simplified in terms of build->domain.
 *
 * The domain of "pa" lives in the external schedule space.
 */
__isl_give isl_ast_expr *isl_ast_build_expr_from_pw_aff(
	__isl_keep isl_ast_build *build, __isl_take isl_pw_aff *pa)
{
	isl_ast_expr *expr;

	if (isl_ast_build_need_schedule_map(build)) {
		isl_multi_aff *ma;
		ma = isl_ast_build_get_schedule_map_multi_aff(build);
		pa = isl_pw_aff_pullback_multi_aff(pa, ma);
	}
	expr = isl_ast_build_expr_from_pw_aff_internal(build, pa);
	return expr;
}

/* Set the ids of the input dimensions of "mpa" to the iterator ids
 * of "build".
 *
 * The domain of "mpa" is assumed to live in the internal schedule domain.
 */
static __isl_give isl_multi_pw_aff *set_iterator_names(
	__isl_keep isl_ast_build *build, __isl_take isl_multi_pw_aff *mpa)
{
	int i, n;

	n = isl_multi_pw_aff_dim(mpa, isl_dim_in);
	for (i = 0; i < n; ++i) {
		isl_id *id;

		id = isl_ast_build_get_iterator_id(build, i);
		mpa = isl_multi_pw_aff_set_dim_id(mpa, isl_dim_in, i, id);
	}

	return mpa;
}

/* Construct an isl_ast_expr of type "type" with as first argument "arg0" and
 * the remaining arguments derived from "mpa".
 * That is, construct a call or access expression that calls/accesses "arg0"
 * with arguments/indices specified by "mpa".
 */
static __isl_give isl_ast_expr *isl_ast_build_with_arguments(
	__isl_keep isl_ast_build *build, enum isl_ast_op_type type,
	__isl_take isl_ast_expr *arg0, __isl_take isl_multi_pw_aff *mpa)
{
	int i, n;
	isl_ctx *ctx;
	isl_ast_expr *expr;

	ctx = isl_ast_build_get_ctx(build);

	n = isl_multi_pw_aff_dim(mpa, isl_dim_out);
	expr = isl_ast_expr_alloc_op(ctx, type, 1 + n);
	expr = isl_ast_expr_set_op_arg(expr, 0, arg0);
	for (i = 0; i < n; ++i) {
		isl_pw_aff *pa;
		isl_ast_expr *arg;

		pa = isl_multi_pw_aff_get_pw_aff(mpa, i);
		arg = isl_ast_build_expr_from_pw_aff_internal(build, pa);
		expr = isl_ast_expr_set_op_arg(expr, 1 + i, arg);
	}

	isl_multi_pw_aff_free(mpa);
	return expr;
}

static __isl_give isl_ast_expr *isl_ast_build_from_multi_pw_aff_internal(
	__isl_keep isl_ast_build *build, enum isl_ast_op_type type,
	__isl_take isl_multi_pw_aff *mpa);

/* Construct an isl_ast_expr that accesses the member specified by "mpa".
 * The range of "mpa" is assumed to be wrapped relation.
 * The domain of this wrapped relation specifies the structure being
 * accessed, while the range of this wrapped relation spacifies the
 * member of the structure being accessed.
 *
 * The domain of "mpa" is assumed to live in the internal schedule domain.
 */
static __isl_give isl_ast_expr *isl_ast_build_from_multi_pw_aff_member(
	__isl_keep isl_ast_build *build, __isl_take isl_multi_pw_aff *mpa)
{
	isl_id *id;
	isl_multi_pw_aff *domain;
	isl_ast_expr *domain_expr, *expr;
	enum isl_ast_op_type type = isl_ast_op_access;

	domain = isl_multi_pw_aff_copy(mpa);
	domain = isl_multi_pw_aff_range_factor_domain(domain);
	domain_expr = isl_ast_build_from_multi_pw_aff_internal(build,
								type, domain);
	mpa = isl_multi_pw_aff_range_factor_range(mpa);
	if (!isl_multi_pw_aff_has_tuple_id(mpa, isl_dim_out))
		isl_die(isl_ast_build_get_ctx(build), isl_error_invalid,
			"missing field name", goto error);
	id = isl_multi_pw_aff_get_tuple_id(mpa, isl_dim_out);
	expr = isl_ast_expr_from_id(id);
	expr = isl_ast_expr_alloc_binary(isl_ast_op_member, domain_expr, expr);
	return isl_ast_build_with_arguments(build, type, expr, mpa);
error:
	isl_multi_pw_aff_free(mpa);
	return NULL;
}

/* Construct an isl_ast_expr of type "type" that calls or accesses
 * the element specified by "mpa".
 * The first argument is obtained from the output tuple name.
 * The remaining arguments are given by the piecewise affine expressions.
 *
 * If the range of "mpa" is a mapped relation, then we assume it
 * represents an access to a member of a structure.
 *
 * The domain of "mpa" is assumed to live in the internal schedule domain.
 */
static __isl_give isl_ast_expr *isl_ast_build_from_multi_pw_aff_internal(
	__isl_keep isl_ast_build *build, enum isl_ast_op_type type,
	__isl_take isl_multi_pw_aff *mpa)
{
	isl_ctx *ctx;
	isl_id *id;
	isl_ast_expr *expr;

	if (!mpa)
		goto error;

	if (type == isl_ast_op_access &&
	    isl_multi_pw_aff_range_is_wrapping(mpa))
		return isl_ast_build_from_multi_pw_aff_member(build, mpa);

	mpa = set_iterator_names(build, mpa);
	if (!build || !mpa)
		goto error;

	ctx = isl_ast_build_get_ctx(build);

	if (isl_multi_pw_aff_has_tuple_id(mpa, isl_dim_out))
		id = isl_multi_pw_aff_get_tuple_id(mpa, isl_dim_out);
	else
		id = isl_id_alloc(ctx, "", NULL);

	expr = isl_ast_expr_from_id(id);
	return isl_ast_build_with_arguments(build, type, expr, mpa);
error:
	isl_multi_pw_aff_free(mpa);
	return NULL;
}

/* Construct an isl_ast_expr of type "type" that calls or accesses
 * the element specified by "pma".
 * The first argument is obtained from the output tuple name.
 * The remaining arguments are given by the piecewise affine expressions.
 *
 * The domain of "pma" is assumed to live in the internal schedule domain.
 */
static __isl_give isl_ast_expr *isl_ast_build_from_pw_multi_aff_internal(
	__isl_keep isl_ast_build *build, enum isl_ast_op_type type,
	__isl_take isl_pw_multi_aff *pma)
{
	isl_multi_pw_aff *mpa;

	mpa = isl_multi_pw_aff_from_pw_multi_aff(pma);
	return isl_ast_build_from_multi_pw_aff_internal(build, type, mpa);
}

/* Construct an isl_ast_expr of type "type" that calls or accesses
 * the element specified by "mpa".
 * The first argument is obtained from the output tuple name.
 * The remaining arguments are given by the piecewise affine expressions.
 *
 * The domain of "mpa" is assumed to live in the external schedule domain.
 */
static __isl_give isl_ast_expr *isl_ast_build_from_multi_pw_aff(
	__isl_keep isl_ast_build *build, enum isl_ast_op_type type,
	__isl_take isl_multi_pw_aff *mpa)
{
	int is_domain;
	isl_ast_expr *expr;
	isl_space *space_build, *space_mpa;

	space_build = isl_ast_build_get_space(build, 0);
	space_mpa = isl_multi_pw_aff_get_space(mpa);
	is_domain = isl_space_tuple_is_equal(space_build, isl_dim_set,
					space_mpa, isl_dim_in);
	isl_space_free(space_build);
	isl_space_free(space_mpa);
	if (is_domain < 0)
		goto error;
	if (!is_domain)
		isl_die(isl_ast_build_get_ctx(build), isl_error_invalid,
			"spaces don't match", goto error);

	if (isl_ast_build_need_schedule_map(build)) {
		isl_multi_aff *ma;
		ma = isl_ast_build_get_schedule_map_multi_aff(build);
		mpa = isl_multi_pw_aff_pullback_multi_aff(mpa, ma);
	}

	expr = isl_ast_build_from_multi_pw_aff_internal(build, type, mpa);
	return expr;
error:
	isl_multi_pw_aff_free(mpa);
	return NULL;
}

/* Construct an isl_ast_expr that calls the domain element specified by "mpa".
 * The name of the function is obtained from the output tuple name.
 * The arguments are given by the piecewise affine expressions.
 *
 * The domain of "mpa" is assumed to live in the external schedule domain.
 */
__isl_give isl_ast_expr *isl_ast_build_call_from_multi_pw_aff(
	__isl_keep isl_ast_build *build, __isl_take isl_multi_pw_aff *mpa)
{
	return isl_ast_build_from_multi_pw_aff(build, isl_ast_op_call, mpa);
}

/* Construct an isl_ast_expr that accesses the array element specified by "mpa".
 * The name of the array is obtained from the output tuple name.
 * The index expressions are given by the piecewise affine expressions.
 *
 * The domain of "mpa" is assumed to live in the external schedule domain.
 */
__isl_give isl_ast_expr *isl_ast_build_access_from_multi_pw_aff(
	__isl_keep isl_ast_build *build, __isl_take isl_multi_pw_aff *mpa)
{
	return isl_ast_build_from_multi_pw_aff(build, isl_ast_op_access, mpa);
}

/* Construct an isl_ast_expr of type "type" that calls or accesses
 * the element specified by "pma".
 * The first argument is obtained from the output tuple name.
 * The remaining arguments are given by the piecewise affine expressions.
 *
 * The domain of "pma" is assumed to live in the external schedule domain.
 */
static __isl_give isl_ast_expr *isl_ast_build_from_pw_multi_aff(
	__isl_keep isl_ast_build *build, enum isl_ast_op_type type,
	__isl_take isl_pw_multi_aff *pma)
{
	isl_multi_pw_aff *mpa;

	mpa = isl_multi_pw_aff_from_pw_multi_aff(pma);
	return isl_ast_build_from_multi_pw_aff(build, type, mpa);
}

/* Construct an isl_ast_expr that calls the domain element specified by "pma".
 * The name of the function is obtained from the output tuple name.
 * The arguments are given by the piecewise affine expressions.
 *
 * The domain of "pma" is assumed to live in the external schedule domain.
 */
__isl_give isl_ast_expr *isl_ast_build_call_from_pw_multi_aff(
	__isl_keep isl_ast_build *build, __isl_take isl_pw_multi_aff *pma)
{
	return isl_ast_build_from_pw_multi_aff(build, isl_ast_op_call, pma);
}

/* Construct an isl_ast_expr that accesses the array element specified by "pma".
 * The name of the array is obtained from the output tuple name.
 * The index expressions are given by the piecewise affine expressions.
 *
 * The domain of "pma" is assumed to live in the external schedule domain.
 */
__isl_give isl_ast_expr *isl_ast_build_access_from_pw_multi_aff(
	__isl_keep isl_ast_build *build, __isl_take isl_pw_multi_aff *pma)
{
	return isl_ast_build_from_pw_multi_aff(build, isl_ast_op_access, pma);
}

/* Construct an isl_ast_expr that calls the domain element
 * specified by "executed".
 *
 * "executed" is assumed to be single-valued, with a domain that lives
 * in the internal schedule space.
 */
__isl_give isl_ast_node *isl_ast_build_call_from_executed(
	__isl_keep isl_ast_build *build, __isl_take isl_map *executed)
{
	isl_pw_multi_aff *iteration;
	isl_ast_expr *expr;

	iteration = isl_pw_multi_aff_from_map(executed);
	iteration = isl_ast_build_compute_gist_pw_multi_aff(build, iteration);
	iteration = isl_pw_multi_aff_intersect_domain(iteration,
					isl_ast_build_get_domain(build));
	expr = isl_ast_build_from_pw_multi_aff_internal(build, isl_ast_op_call,
							iteration);
	return isl_ast_node_alloc_user(expr);
}
