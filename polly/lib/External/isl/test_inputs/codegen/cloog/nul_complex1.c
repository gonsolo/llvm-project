for (int c0 = 0; c0 <= 5 * n; c0 += 1)
  for (int c1 = max(-((5 * n - c0 + 1) % 2) - n + c0 + 1, 2 * floord(c0 - 1, 3) + 2); c1 <= min(c0, n + c0 - (n + c0 + 2) / 3); c1 += 2)
    S1((3 * c1 / 2) - c0, c0 - c1);
