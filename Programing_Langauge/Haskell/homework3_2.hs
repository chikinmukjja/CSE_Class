
  


filpCoin list n = changeFace (take n list) ++ ( drop n list )  


equalCoin list m
	| length list ==  m = []
	| last (take m list) == head (drop m list) = equalCoin list (m+1)
	| otherwise = filpCoin list m 		

flipCoin list 
 | (take (length list) (repeat 'H')) == list = [0]  
 | (take (length list) (repeat 'T')) == list = (length list):flipCoin(equalCoin list (length list))
 |  otherwise = (flipWhere list 1):flipCoin(equalCoin list 1)
 

flipWhere x y
        | last (take y x) /= last (take (y+1) x) = y
	| otherwise = flipWhere x (y+1)



changeFace face = get face
		   where
		   
		get [] = [] 
		get [x] = oneFace x :[]
		get (p:xs) =  oneFace p  : (get xs) 
		        
		  
oneFace face 
 | face == 'H' = 'T'
 | face == 'T' = 'H'
 | otherwise = 'E'

