line list depth height
 | list == [] = depth
 | head list == '(' = line (tail list) (max depth height) (height+1) 
 | head list == ')' = line (tail list) (max depth height) (height-1) 
					

makeCase n k output -- n갯만큼의 가로를 가진 string 조합
 | (n > 0 && k==1) = (makeCase (n-1) 1 ('(':output)) ++ (makeCase (n-1) 2 (')':output))
 | (n > 0 && k==2) = (makeCase (n-1) 2 (')':output)) ++ (makeCase (n-1) 1 ('(':output))
 | otherwise = output

makeList n = divCase n (makeCase n 1 []) []  

checkList xs =  [ x | x <- xs,head x == '(', last x == ')',(howManyLeft x 0) == (howManyRight x 0), isError x (length x) [] ]  

expression n m  =  length (exp2 (check n) m) 

exp2 xs m = [x | x <-xs, (line x 0 0 )== m ] 

check n = checkList ( makeList n )   
 --
isError list n stack 
 | ( n == 0 && null stack ) = True
 | ( n == 0 && null stack ) = False
 |  null list = False 
 | head list == '(' = isError (tail list) (n-1) ('(':stack)
 | head list == ')' = isError (tail list) (n-1) (pop stack) 
			where pop list2
	 			| ( head list2 == '(') = tail list2  
				| otherwise =  list2

howManyLeft list n 			 
 | list == [] = n
 | head list == '(' = howManyLeft (tail list) (n+1) 
 | head list == ')' = howManyLeft (tail list) n

howManyRight list n
 | list == [] = n
 | head list == ')' = howManyRight (tail list) (n+1)
 | head list == '(' = howManyRight (tail list) n


divCase n input output
 | input == [] = output
 | otherwise = divCase n (drop n input) ((take n input):output)




