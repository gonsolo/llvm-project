if (P1 >= 0 && P1 <= 3 && P2 >= 0 && P2 <= 3)
  for (int c0 = P1 - 1; c0 <= 3; c0 += 1)
    for (int c2 = 0; c2 <= 7; c2 += 1)
      for (int c3 = 0; c3 <= 7; c3 += 1)
        if ((5 * P2 + 2 * c3) % 9 <= 3) {
          if (P1 >= 1 && c0 + 1 == P1 && (5 * P1 + 2 * c2) % 9 <= 2) {
            s0(P1 - 1, P2, c2, c3, ((5 * P1 + 2 * c2 + 9) % 9) + 1, -4 * P2 + 2 * c3 - 9 * floord(-4 * P2 + 2 * c3, 9));
          } else if (P1 == 0 && c0 == 3 && c2 % 4 == 0)
            s0(3, P2, c2, c3, (-c2 / 4) + 3, -4 * P2 + 2 * c3 - 9 * floord(-4 * P2 + 2 * c3, 9));
        }
