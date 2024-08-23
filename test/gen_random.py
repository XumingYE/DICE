import random

random_numbers = [str(random.uniform(1.2, 1.32)) for _ in range(5)]
print('\t'.join(random_numbers))