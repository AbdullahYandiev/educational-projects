def hello(x = None):
    if x == None or x == "":
        return "Hello!"
    else:
        return "Hello, {}!".format(x)