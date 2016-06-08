
primes = getPrime[2..]
	where
	getPrime (p:xs) = p : getPrime[x | x <- xs, x `mod` p > 0]

findingPrimes n size  
		| n < 1 = take size (filter ( > 1) primes )
		| size < 1 = []  
		|  otherwise = take size (filter ( > n) primes )