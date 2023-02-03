def longestCommonPrefix(x):
    if len(x) == 0:
        return ""
    res = ""
    for i, c in enumerate(x[0].lstrip()):
        for s in x[1:]:
            s = s.lstrip()
            if not (len(s) > i and s[i] == c):
                return res
        res += c
    return res