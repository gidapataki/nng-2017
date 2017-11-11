import random
import argparse

def make_testcase():
    print 1
    type_count = random.randint(0, 5)
    print ' '.join([str(random.randint(1, 20)), str(type_count)])
    for i in range(type_count):
        print ' '.join([str(random.randint(1, 20)), str(random.randint(1, 4))])


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('seed', type=int, help='seed')

    args = parser.parse_args()
    random.seed(args.seed);

    make_testcase()
