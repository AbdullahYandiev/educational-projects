def primes():
    yield 2
    l, k = [2], 3
    while True:
        for i in l:
            if k % i == 0:
                break
        else:
            yield k
            l.append(k)
        k += 1