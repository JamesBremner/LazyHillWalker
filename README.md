# LazyHillWalker
Find easiest path through hilly terrain

Search for a path through a rectangular grid of heights that minimizes the changes in elevation.  The cost of a change in elevation is defined to be

sqrt(1+(height(a)-(height(b))^2)

## Input

A space delimited text file.

### Elevation grid

Specifies elevation of an m row and n column grid
| Column | Desscription | 
|---|---|
 1 | "h", for height
 2 ... n+1 | height for each location in a row n wide 

### Starting and ending locations

| Column | Desscription | 
|---|---|
1 | "s", for start
2 | x location column
3 | y location row

| Column | Desscription | 
|---|---|
1 | "e", for end
2 | x location column
3 | y location row
