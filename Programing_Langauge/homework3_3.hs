increment '0' = '1'
increment '1' = '2'
increment '2' = '3'
increment '3' = '4'
increment '4' = '5'
increment '5' = '6'
increment '6' = '7'
increment '7' = '8'
increment '8' = '8'
increment '/' = '/'
increment '*' = '*'

initMap size 
 | size > 0 = '0':initMap (size-1) 
 | size == 0 = []

ifstar list star = (take (star-1) list) ++ ['*'] ++ drop star list
ifslash list slash = (take (slash-1) list) ++ ['/'] ++ drop slash list

increN  pos size list
 | pos < 1 = list
 | pos > size = list
 | otherwise = (take (pos-1) list ) ++ [increment (last (take pos list)) ] ++ drop pos list

makeMap input output index size row
 | index == size = output
 | head input == '.' = makeMap (tail input) output (index+1) size row
 | head input == '/' = makeMap (tail input) (ifslash output index) (index+1) size row
 | head input == '*' = makeMap (tail input) (ifstar eightWay index) (index+1) size row where 
	eightWay = increN (index+row+1) size (
		   increN (index+row) size (
		   increN (index+row-1) size (
                   increN (index+1) size (
                   increN (index-1) size (
                   increN (index-row+1) size  (
                   increN (index-row) size (
                   increN (index-row-1) size output)))))))   


minesweep m n input = makeMap input (initMap (((n+1)*m)-1)) 1 ((n+1)*m) (n+1)