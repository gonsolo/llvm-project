for (int c0 = 0; c0 < m; c0 += 32)
  for (int c1 = (n >= 32 && m >= c0 + 2) || (m == 1 && c0 == 0) ? 0 : 32 * n - 32 * floord(31 * n + 31, 32); c1 <= ((n <= -1 && c0 == 0) || (m == 1 && n >= 0 && c0 == 0) ? max(0, n - 1) : n); c1 += 32)
    for (int c2 = c0; c2 <= (m >= 2 && c0 + 31 >= m && n >= c1 && c1 + 31 >= n ? 2 * m - 3 : (m >= 2 * c0 + 63 && c1 <= -32 && n >= c1 && c1 + 31 >= n) || (m >= c0 + 32 && 2 * c0 + 62 >= m && n >= c1 && c1 + 31 >= n) || (n >= 0 && c0 >= 32 && m >= 2 * c0 + 63 && c1 == n) || (m >= 63 && n >= 32 && c0 == 0 && c1 == n) ? 2 * c0 + 61 : m - 1); c2 += 32) {
      if (c1 >= n) {
        if (c0 == 0 && c1 == 0)
          for (int c5 = 0; c5 <= min(31, m - c2 - 1); c5 += 1)
            S_14(c2 + c5);
        if (c1 == n)
          for (int c3 = max(0, (c2 / 2) - c0 + 1); c3 <= min(31, m - c0 - 1); c3 += 1)
            for (int c5 = max(0, c0 - c2 + c3); c5 <= min(31, 2 * c0 - c2 + 2 * c3 - 1); c5 += 1)
              S_29(-c0 + c2 - c3 + c5, c0 + c3);
      } else if (c1 >= 0 && m >= c2 + 1) {
        for (int c3 = 0; c3 <= min(min(31, m - c0 - 2), -c0 + c2 + 30); c3 += 1) {
          for (int c4 = 0; c4 <= min(31, n - c1 - 1); c4 += 1) {
            if (c0 == 0 && c2 == 0 && c3 == 0) {
              if (c1 == 0 && c4 == 0)
                S_14(0);
              S_19(c1 + c4, 0);
            }
            for (int c5 = max(0, c0 - c2 + c3 + 1); c5 <= min(31, m - c2 - 1); c5 += 1) {
              if (c0 == 0 && c1 == 0 && c3 == 0 && c4 == 0)
                S_14(c2 + c5);
              if (c0 == 0 && c3 == 0)
                S_19(c1 + c4, c2 + c5);
              S_27(c0 + c3, c2 + c5, c1 + c4);
            }
          }
          if (c1 + 31 >= n)
            for (int c5 = max(0, c0 - c2 + c3); c5 <= min(31, 2 * c0 - c2 + 2 * c3 - 1); c5 += 1)
              S_29(-c0 + c2 - c3 + c5, c0 + c3);
        }
        if (c0 + 31 >= m && c1 + 31 >= n && c2 == c0) {
          for (int c5 = m - c0 - 1; c5 <= min(31, 2 * m - c0 - 3); c5 += 1)
            S_29(-m + c0 + c5 + 1, m - 1);
        } else if (m >= c0 + 32 && c1 + 31 >= n && c2 == c0)
          S_29(0, c0 + 31);
      } else if (c1 + 31 >= n && c2 >= m) {
        for (int c3 = max(0, (c2 / 2) - c0 + 1); c3 <= min(31, m - c0 - 1); c3 += 1)
          for (int c5 = 0; c5 <= min(31, 2 * c0 - c2 + 2 * c3 - 1); c5 += 1)
            S_29(-c0 + c2 - c3 + c5, c0 + c3);
      } else if (c1 <= -32 && c1 + 31 >= n && m >= c2 + 1)
        for (int c3 = max(0, (c2 / 2) - c0 + 1); c3 <= min(31, m - c0 - 1); c3 += 1)
          for (int c5 = max(0, c0 - c2 + c3); c5 <= min(31, 2 * c0 - c2 + 2 * c3 - 1); c5 += 1)
            S_29(-c0 + c2 - c3 + c5, c0 + c3);
      if (m == 1 && c0 == 0 && c1 >= 32 && c2 == 0) {
        for (int c4 = 0; c4 <= min(31, n - c1 - 1); c4 += 1)
          S_19(c1 + c4, 0);
      } else if (m == 1 && n >= 1 && c0 == 0 && c1 == 0 && c2 == 0) {
        S_14(0);
        for (int c4 = 0; c4 <= min(31, n - 1); c4 += 1)
          S_19(c4, 0);
      }
    }
