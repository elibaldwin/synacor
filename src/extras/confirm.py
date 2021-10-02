# %%
from functools import lru_cache

@lru_cache(maxsize=None)
def f(n, m, c):
  if n == 0:
    return m+1
  if m == 0:
    return f(n-1, c, c)
  return f(n-1, f(n, m-1, c), c)
# %%

import numpy as np

def calc_fn(n, m, c, cache):
  for j in range(1, n+1):
    cache[j,0] = cache[j-1,c]
    for i in range(1, 2**15):
      sub = cache[j, i-1] % (2**15)
      cache[j,i] = cache[j-1, sub] 
  return cache[n, m]

cache = np.zeros((5, 2**15), dtype=np.int16)
for i in range(2**15):
  cache[0,i] = i+1

for i in range(1, 2**15):
  if i % 10 == 0:
    print(i)
  if calc_fn(4, 1, i, cache) == 6:
    print(i)
    break
# %%
