# RUN: llvm-mc %s -triple=mips-unknown-linux -show-encoding -mcpu=mips32r6 -mattr=micromips -mattr=+dspr2 | FileCheck %s

  .set noat
  absq_s.ph $3, $4             # CHECK: absq_s.ph $3, $4        # encoding: [0x00,0x64,0x11,0x3c]
  absq_s.qb $3, $4             # CHECK: absq_s.qb $3, $4        # encoding: [0x00,0x64,0x01,0x3c]
  absq_s.w $3, $4              # CHECK: absq_s.w $3, $4         # encoding: [0x00,0x64,0x21,0x3c]
  addqh.ph $3, $4, $5          # CHECK: addqh.ph $3, $4, $5     # encoding: [0x00,0xa4,0x18,0x4d]
  addqh_r.ph $3, $4, $5        # CHECK: addqh_r.ph $3, $4, $5   # encoding: [0x00,0xa4,0x1c,0x4d]
  addqh.w $3, $4, $5           # CHECK: addqh.w $3, $4, $5      # encoding: [0x00,0xa4,0x18,0x8d]
  addqh_r.w $3, $4, $5         # CHECK: addqh_r.w $3, $4, $5    # encoding: [0x00,0xa4,0x1c,0x8d]
  addu.ph $3, $4, $5           # CHECK: addu.ph $3, $4, $5      # encoding: [0x00,0xa4,0x19,0x0d]
  addu.qb $3, $4, $5           # CHECK: addu.qb $3, $4, $5      # encoding: [0x00,0xa4,0x18,0xcd]
  addu_s.ph $3, $4, $5         # CHECK: addu_s.ph $3, $4, $5    # encoding: [0x00,0xa4,0x1d,0x0d]
  addu_s.qb $3, $4, $5         # CHECK: addu_s.qb $3, $4, $5    # encoding: [0x00,0xa4,0x1c,0xcd]
  adduh.qb $3, $4, $5          # CHECK: adduh.qb $3, $4, $5     # encoding: [0x00,0xa4,0x19,0x4d]
  adduh_r.qb $3, $4, $5        # CHECK: adduh_r.qb $3, $4, $5   # encoding: [0x00,0xa4,0x1d,0x4d]
  addsc $3, $4, $5             # CHECK: addsc $3, $4, $5        # encoding: [0x00,0xa4,0x1b,0x85]
  addwc $3, $4, $5             # CHECK: addwc $3, $4, $5        # encoding: [0x00,0xa4,0x1b,0xc5]
  addq.ph $3, $4, $5           # CHECK: addq.ph $3, $4, $5      # encoding: [0x00,0xa4,0x18,0x0d]
  addq_s.ph $3, $4, $5         # CHECK: addq_s.ph $3, $4, $5    # encoding: [0x00,0xa4,0x1c,0x0d]
  addq_s.w $3, $4, $5          # CHECK: addq_s.w $3, $4, $5     # encoding: [0x00,0xa4,0x1b,0x05]
  dpa.w.ph $ac0, $3, $2        # CHECK: dpa.w.ph $ac0, $3, $2      # encoding: [0x00,0x43,0x00,0xbc]
  dpaq_s.w.ph $ac1, $5, $3     # CHECK: dpaq_s.w.ph $ac1, $5, $3   # encoding: [0x00,0x65,0x42,0xbc]
  dpaq_sa.l.w $ac2, $4, $3     # CHECK: dpaq_sa.l.w $ac2, $4, $3   # encoding: [0x00,0x64,0x92,0xbc]
  dpaqx_s.w.ph $ac3, $12, $7   # CHECK: dpaqx_s.w.ph $ac3, $12, $7 # encoding: [0x00,0xec,0xe2,0xbc]
  dpaqx_sa.w.ph $ac0, $5, $6   # CHECK: dpaqx_sa.w.ph $ac0, $5, $6 # encoding: [0x00,0xc5,0x32,0xbc]
  dpau.h.qbl $ac1, $3, $4      # CHECK: dpau.h.qbl $ac1, $3, $4    # encoding: [0x00,0x83,0x60,0xbc]
  dpau.h.qbr $ac2, $20, $21    # CHECK: dpau.h.qbr $ac2, $20, $21  # encoding: [0x02,0xb4,0xb0,0xbc]
  dpax.w.ph $ac3, $2, $1       # CHECK: dpax.w.ph $ac3, $2, $1     # encoding: [0x00,0x22,0xd0,0xbc]
  extp $zero, $ac1, 6          # CHECK: extp $zero, $ac1, 6        # encoding: [0x00,0x06,0x66,0x7c]
  extpdp $2, $ac1, 2           # CHECK: extpdp $2, $ac1, 2         # encoding: [0x00,0x42,0x76,0x7c]
  extpdpv $4, $ac2, $8         # CHECK: extpdpv $4, $ac2, $8       # encoding: [0x00,0x88,0xb8,0xbc]
  extpv $15, $ac3, $7          # CHECK: extpv $15, $ac3, $7        # encoding: [0x01,0xe7,0xe8,0xbc]
  extr.w $27, $ac3, 31         # CHECK: extr.w $27, $ac3, 31       # encoding: [0x03,0x7f,0xce,0x7c]
  extr_r.w $12, $ac0, 24       # CHECK: extr_r.w $12, $ac0, 24     # encoding: [0x01,0x98,0x1e,0x7c]
  extr_rs.w $27, $ac3, 9       # CHECK: extr_rs.w $27, $ac3, 9     # encoding: [0x03,0x69,0xee,0x7c]
  extr_s.h $3, $ac2, 1         # CHECK: extr_s.h $3, $ac2, 1       # encoding: [0x00,0x61,0xbe,0x7c]
  extrv.w $5, $ac0, $6         # CHECK: extrv.w $5, $ac0, $6       # encoding: [0x00,0xa6,0x0e,0xbc]
  extrv_r.w $10, $ac0, $3      # CHECK: extrv_r.w $10, $ac0, $3    # encoding: [0x01,0x43,0x1e,0xbc]
  extrv_rs.w $15, $ac1, $20    # CHECK: extrv_rs.w $15, $ac1, $20  # encoding: [0x01,0xf4,0x6e,0xbc]
  extrv_s.h $8, $ac2, $16      # CHECK: extrv_s.h $8, $ac2, $16    # encoding: [0x01,0x10,0xbe,0xbc]
  insv $3, $4                  # CHECK: insv $3, $4             # encoding: [0x00,0x64,0x41,0x3c]
  madd $ac1, $6, $7            # CHECK: madd $ac1, $6, $7       # encoding: [0x00,0xe6,0x4a,0xbc]
  maddu $ac0, $8, $9           # CHECK: maddu $ac0, $8, $9      # encoding: [0x01,0x28,0x1a,0xbc]
  msub $ac3, $10, $11          # CHECK: msub $ac3, $10, $11     # encoding: [0x01,0x6a,0xea,0xbc]
  msubu $ac2, $12, $13         # CHECK: msubu $ac2, $12, $13    # encoding: [0x01,0xac,0xba,0xbc]
  mult $ac3, $2, $3            # CHECK: mult $ac3, $2, $3       # encoding: [0x00,0x62,0xcc,0xbc]
  multu $ac2, $4, $5           # CHECK: multu $ac2, $4, $5      # encoding: [0x00,0xa4,0x9c,0xbc]
  packrl.ph $3, $4, $5         # CHECK: packrl.ph $3, $4, $5    # encoding: [0x00,0xa4,0x19,0xad]
  pick.ph $3, $4, $5           # CHECK: pick.ph $3, $4, $5      # encoding: [0x00,0xa4,0x1a,0x2d]
  pick.qb $3, $4, $5           # CHECK: pick.qb $3, $4, $5      # encoding: [0x00,0xa4,0x19,0xed]
  preceq.w.phl $1, $2          # CHECK: preceq.w.phl $1, $2      # encoding: [0x00,0x22,0x51,0x3c]
  preceq.w.phr $3, $4          # CHECK: preceq.w.phr $3, $4      # encoding: [0x00,0x64,0x61,0x3c]
  precequ.ph.qbl $5, $6        # CHECK: precequ.ph.qbl $5, $6    # encoding: [0x00,0xa6,0x71,0x3c]
  precequ.ph.qbla $7, $8       # CHECK: precequ.ph.qbla $7, $8   # encoding: [0x00,0xe8,0x73,0x3c]
  precequ.ph.qbr $9, $10       # CHECK: precequ.ph.qbr $9, $10   # encoding: [0x01,0x2a,0x91,0x3c]
  precequ.ph.qbra $11, $12     # CHECK: precequ.ph.qbra $11, $12 # encoding: [0x01,0x6c,0x93,0x3c]
  preceu.ph.qbl $13, $14       # CHECK: preceu.ph.qbl $13, $14   # encoding: [0x01,0xae,0xb1,0x3c]
  preceu.ph.qbla $15, $16      # CHECK: preceu.ph.qbla $15, $16  # encoding: [0x01,0xf0,0xb3,0x3c]
  preceu.ph.qbr $17, $18       # CHECK: preceu.ph.qbr $17, $18   # encoding: [0x02,0x32,0xd1,0x3c]
  preceu.ph.qbra $19, $20      # CHECK: preceu.ph.qbra $19, $20  # encoding: [0x02,0x74,0xd3,0x3c]
  precr.qb.ph $1, $2, $3        # CHECK: precr.qb.ph $1, $2, $3        # encoding: [0x00,0x62,0x08,0x6d]
  precr_sra.ph.w $4, $5, 1      # CHECK: precr_sra.ph.w $4, $5, 1      # encoding: [0x00,0x85,0x0b,0xcd]
  precr_sra_r.ph.w $6, $7, 2    # CHECK: precr_sra_r.ph.w $6, $7, 2    # encoding: [0x00,0xc7,0x17,0xcd]
  precrq.ph.w $8, $9, $10       # CHECK: precrq.ph.w $8, $9, $10       # encoding: [0x01,0x49,0x40,0xed]
  precrq.qb.ph $11, $12, $13    # CHECK: precrq.qb.ph $11, $12, $13    # encoding: [0x01,0xac,0x58,0xad]
  precrqu_s.qb.ph $14, $15, $16 # CHECK: precrqu_s.qb.ph $14, $15, $16 # encoding: [0x02,0x0f,0x71,0x6d]
  precrq_rs.ph.w $17, $18, $19  # CHECK: precrq_rs.ph.w $17, $18, $19  # encoding: [0x02,0x72,0x89,0x2d]
  shilo $ac1, 3                # CHECK: shilo $ac1, 3           # encoding: [0x00,0x03,0x40,0x1d]
  shilov $ac1, $5              # CHECK: shilov $ac1, $5         # encoding: [0x00,0x05,0x52,0x7c]
  shll.ph $3, $4, 5            # CHECK: shll.ph $3, $4, 5       # encoding: [0x00,0x64,0x53,0xb5]
  shll_s.ph $3, $4, 5          # CHECK: shll_s.ph $3, $4, 5     # encoding: [0x00,0x64,0x5b,0xb5]
  shll.qb $3, $4, 5            # CHECK: shll.qb $3, $4, 5       # encoding: [0x00,0x64,0xa8,0x7c]
  shllv.ph $3, $4, $5          # CHECK: shllv.ph $3, $4, $5     # encoding: [0x00,0x85,0x18,0x0e]
  shllv_s.ph $3, $4, $5        # CHECK: shllv_s.ph $3, $4, $5   # encoding: [0x00,0x85,0x1c,0x0e]
  shllv.qb $3, $4, $5          # CHECK: shllv.qb $3, $4, $5     # encoding: [0x00,0x85,0x1b,0x95]
  shllv_s.w $3, $4, $5         # CHECK: shllv_s.w $3, $4, $5    # encoding: [0x00,0x85,0x1b,0xd5]
  shll_s.w $3, $4, 5           # CHECK: shll_s.w $3, $4, 5      # encoding: [0x00,0x64,0x2b,0xf5]
  shra.ph $3, $4, 5            # CHECK: shra.ph $3, $4, 5       # encoding: [0x00,0x64,0x53,0x35]
  shra.qb $3, $4, 5            # CHECK: shra.qb $3, $4, 5       # encoding: [0x00,0x64,0xa1,0xfc]
  shra_r.ph $3, $4, 5          # CHECK: shra_r.ph $3, $4, 5     # encoding: [0x00,0x64,0x57,0x35]
  shra_r.qb $3, $4, 5          # CHECK: shra_r.qb $3, $4, 5     # encoding: [0x00,0x64,0xb1,0xfc]
  shrav.ph $3, $4, $5          # CHECK: shrav.ph $3, $4, $5     # encoding: [0x00,0x85,0x19,0x8d]
  shrav.qb $3, $4, $5          # CHECK: shrav.qb $3, $4, $5     # encoding: [0x00,0x85,0x19,0xcd]
  shrav_r.ph $3, $4, $5        # CHECK: shrav_r.ph $3, $4, $5   # encoding: [0x00,0x85,0x1d,0x8d]
  shrav_r.qb $3, $4, $5        # CHECK: shrav_r.qb $3, $4, $5   # encoding: [0x00,0x85,0x1d,0xcd]
  shrav_r.w $3, $4, $5         # CHECK: shrav_r.w $3, $4, $5    # encoding: [0x00,0x85,0x1a,0xd5]
  shra_r.w $3, $4, 5           # CHECK: shra_r.w $3, $4, 5      # encoding: [0x00,0x64,0x2a,0xf5]
  shrl.ph $3, $4, 5            # CHECK: shrl.ph $3, $4, 5       # encoding: [0x00,0x64,0x53,0xfc]
  shrl.qb $3, $4, 5            # CHECK: shrl.qb $3, $4, 5       # encoding: [0x00,0x64,0xb8,0x7c]
  shrlv.ph $3, $4, $5          # CHECK: shrlv.ph $3, $4, $5     # encoding: [0x00,0x85,0x1b,0x15]
  shrlv.qb $3, $4, $5          # CHECK: shrlv.qb $3, $4, $5     # encoding: [0x00,0x85,0x1b,0x55]
  subq.ph $3, $4, $5           # CHECK: subq.ph $3, $4, $5      # encoding: [0x00,0xa4,0x1a,0x0d]
  subq_s.ph $3, $4, $5         # CHECK: subq_s.ph $3, $4, $5    # encoding: [0x00,0xa4,0x1e,0x0d]
  subq_s.w $3, $4, $5          # CHECK: subq_s.w $3, $4, $5     # encoding: [0x00,0xa4,0x1b,0x45]
  subqh.ph $3, $4, $5          # CHECK: subqh.ph $3, $4, $5     # encoding: [0x00,0xa4,0x1a,0x4d]
  subqh_r.ph $3, $4, $5        # CHECK: subqh_r.ph $3, $4, $5   # encoding: [0x00,0xa4,0x1e,0x4d]
  subqh.w $3, $4, $5           # CHECK: subqh.w $3, $4, $5      # encoding: [0x00,0xa4,0x1a,0x8d]
  subqh_r.w $3, $4, $5         # CHECK: subqh_r.w $3, $4, $5    # encoding: [0x00,0xa4,0x1e,0x8d]
  subu.ph $3, $4, $5           # CHECK: subu.ph $3, $4, $5      # encoding: [0x00,0xa4,0x1b,0x0d]
  subu_s.ph $3, $4, $5         # CHECK: subu_s.ph $3, $4, $5    # encoding: [0x00,0xa4,0x1f,0x0d]
  subu.qb $3, $4, $5           # CHECK: subu.qb $3, $4, $5      # encoding: [0x00,0xa4,0x1a,0xcd]
  subu_s.qb $3, $4, $5         # CHECK: subu_s.qb $3, $4, $5    # encoding: [0x00,0xa4,0x1e,0xcd]
  subuh.qb $3, $4, $5          # CHECK: subuh.qb $3, $4, $5     # encoding: [0x00,0xa4,0x1b,0x4d]
  subuh_r.qb $3, $4, $5        # CHECK: subuh_r.qb $3, $4, $5   # encoding: [0x00,0xa4,0x1f,0x4d]
  dpsq_s.w.ph $ac1, $4, $6     # CHECK: dpsq_s.w.ph $ac1, $4, $6   # encoding: [0x00,0xc4,0x46,0xbc]
  dpsq_sa.l.w $ac1, $4, $6     # CHECK: dpsq_sa.l.w $ac1, $4, $6   # encoding: [0x00,0xc4,0x56,0xbc]
  dpsu.h.qbl $ac1, $4, $6      # CHECK: dpsu.h.qbl $ac1, $4, $6    # encoding: [0x00,0xc4,0x64,0xbc]
  dpsu.h.qbr $ac1, $4, $6      # CHECK: dpsu.h.qbr $ac1, $4, $6    # encoding: [0x00,0xc4,0x74,0xbc]
  dps.w.ph $ac1, $4, $6        # CHECK: dps.w.ph $ac1, $4, $6      # encoding: [0x00,0xc4,0x44,0xbc]
  dpsqx_s.w.ph $ac1, $4, $6    # CHECK: dpsqx_s.w.ph $ac1, $4, $6  # encoding: [0x00,0xc4,0x66,0xbc]
  dpsqx_sa.w.ph $ac1, $4, $6   # CHECK: dpsqx_sa.w.ph $ac1, $4, $6 # encoding: [0x00,0xc4,0x76,0xbc]
  dpsx.w.ph $ac1, $4, $6       # CHECK: dpsx.w.ph $ac1, $4, $6     # encoding: [0x00,0xc4,0x54,0xbc]
  mul.ph $1, $2, $3            # CHECK: mul.ph $1, $2, $3      # encoding: [0x00,0x62,0x08,0x2d]
  mul_s.ph $1, $2, $3          # CHECK: mul_s.ph $1, $2, $3    # encoding: [0x00,0x62,0x0c,0x2d]
  mulq_rs.w $1, $2, $3         # CHECK: mulq_rs.w $1, $2, $3   # encoding: [0x00,0x62,0x09,0x95]
  mulq_s.ph $1, $2, $3         # CHECK: mulq_s.ph $1, $2, $3   # encoding: [0x00,0x62,0x09,0x55]
  mulq_s.w $1, $2, $3          # CHECK: mulq_s.w $1, $2, $3    # encoding: [0x00,0x62,0x09,0xd5]
  muleq_s.w.phl $1, $2, $3     # CHECK: muleq_s.w.phl $1, $2, $3   # encoding: [0x00,0x62,0x08,0x25]
  muleq_s.w.phr $1, $2, $3     # CHECK: muleq_s.w.phr $1, $2, $3   # encoding: [0x00,0x62,0x08,0x65]
  muleu_s.ph.qbl $1, $2, $3    # CHECK: muleu_s.ph.qbl $1, $2, $3  # encoding: [0x00,0x62,0x08,0x95]
  muleu_s.ph.qbr $1, $2, $3    # CHECK: muleu_s.ph.qbr $1, $2, $3  # encoding: [0x00,0x62,0x08,0xd5]
  mulq_rs.ph $1, $2, $3        # CHECK: mulq_rs.ph $1, $2, $3      # encoding: [0x00,0x62,0x09,0x15]
  prepend $1, $2, 3            # CHECK: prepend $1, $2, 3          # encoding: [0x00,0x22,0x1a,0x55]
  wrdsp $5                     # CHECK: wrdsp $5                # encoding: [0x00,0xa7,0xd6,0x7c]
  wrdsp $5, 2                  # CHECK: wrdsp $5, 2             # encoding: [0x00,0xa0,0x96,0x7c]
  wrdsp $5, 31                 # CHECK: wrdsp $5                # encoding: [0x00,0xa7,0xd6,0x7c]
