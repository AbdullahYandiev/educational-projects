def check(x, file):
    with open(file, 'w') as f:
        d = {}
        for s in x.lower().split():
            d[s] = 1 if s not in d else d[s] + 1
        for k in sorted(d):
            f.write(k + " {}\n".format(d[k]))