# %%
from collections import deque

grid = [['22', '-', '9', '*'],
        ['+', '4', '-', '18'],
        ['4', '*', '11', '*'],
        ['*', '8', '-', '1']]

def adj(x,y, path, grid):
  for dx, dy in [(0,1), (1,0), (0,-1), (-1, 0)]:
    nx,ny = x+dx, y+dy
    if (0 <= nx < 4) and (0 <= ny < 4) and not (nx == 0 and ny == 0):
      if (nx % 2 == ny % 2):
        yield (nx, ny, str(eval(path+grid[ny][nx])))
      else:
        yield (nx, ny, path + grid[ny][nx])

def backtrack(state, prev):
  x,y,_ = state
  locs = []
  locs.append((x,y))
  while state in prev:
    state = prev[state]
    x,y,_ = state
    locs.append((x,y))
  locs.reverse()
  return locs

Q = deque()
Q.append((0, 0, '22'))

seen = set()

prev = dict()

final_path = None

while Q:
  x,y,path = Q.popleft()

  if x == 3 and y == 3:
    if path == '30':
      final_path = backtrack((x,y,path), prev)
      break
    else:
      continue
  
  for move in adj(x,y,path,grid):
    if move not in seen:
      Q.append(move)
      seen.add(move)
      prev[move] = (x,y,path)


# %%
s = ''
for x,y in final_path:
  s += grid[y][x]
s
# %%

prev = final_path[0]
dirs = {(0,1):'north', (0, -1):'south', (-1,0):'west', (1,0):'east'}
dir_path = []
for x,y in final_path[1:]:
  px,py = prev
  d = (x-px, y-py)
  dir_path.append(dirs[d])
  prev = (x,y)
print(dir_path)
# %%
with open('vault_steps.txt', 'w') as f:
  for step in dir_path:
    f.write(step + '\n')
# %%
