def is_palindrome(x):
   t, rev = x, 0
   while t != 0:
      rev = rev * 10 + t % 10
      t //= 10
   return "YES" if x == rev else "NO"