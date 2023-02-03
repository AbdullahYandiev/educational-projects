class BankCard:
    def __init__(self, total_sum):
        self.total_sum = total_sum

    def __call__(self, x):
        if x > self.total_sum:
            raise ValueError('Not enough money to spent {} dollars.'.format(x))
        self.total_sum -= x
        print('You spent {} dollars. {} dollars are left.'. format(x, self.total_sum))

    def __repr__(self):
        return 'To learn the balance you should put the money on the card, spent some money or get the bank data. The last procedure is not free and costs 1 dollar.'

    @property
    def balance(self):
        if self.total_sum < 1:
            raise ValueError('Not enough money to learn the balance.')
        self.total_sum -= 1
        return self.total_sum

    def put(self, x):
        self.total_sum += x
        print('You put {} dollars. {} dollars are left.'.format(x, self.total_sum))