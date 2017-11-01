#!/usr/bin/env python3

import argparse
import random

def generate_fraction():
    p = random.randint(1, 100)
    q = random.randint(1, 10)
    return str(p) + "/" + str(q)

def generate_input(racers, bookies):
    print(str(racers) + " " + str(bookies))
    for racer in range(racers):
        print(str.join(" ", [generate_fraction() for _ in range(bookies)]))

    bookieLimits = [random.randint(1, min(5, racers)) for _ in range(bookies)]
    print(str.join(" ", [str(bookieLimit) for bookieLimit in bookieLimits]))


def main():
    parser = argparse.ArgumentParser(
        description="Generate input for sports-betting")
    parser.add_argument(
        "-c", "--count", metavar="N", type=int, default=1,
        help="How many test inputs to generate")
    parser.add_argument(
        "-n", "--racers", metavar="N", type=int, default=3,
        help="How many racers should be running")
    parser.add_argument(
        "-m", "--bookies", metavar="N", type=int, default=2,
        help="How many bookies should accept bets")
    parser.add_argument(
        "-s", "--seed", metavar="N", type=int, default=0,
        help="What to seed the RNG with")
    args = parser.parse_args()

    random.seed(args.seed)

    number_of_inputs = args.count
    print(number_of_inputs)
    for i in range(number_of_inputs):
        generate_input(args.racers, args.bookies)


if __name__ == "__main__":
    main()
