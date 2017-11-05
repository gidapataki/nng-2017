#!/usr/bin/env python3
import argparse
from random import seed, randint, shuffle
from fractions import Fraction


def generate_fraction():
    p = randint(1, 100)
    q = randint(1, 10)
    return Fraction(p, q)


def generate_input(racers, bookies):
    print(str(racers) + " " + str(bookies))
    bookie_limits = [randint(1, min(5, racers)) for _ in range(bookies)]
    bet_slots = []
    total_slots = 0
    for bookie in range(bookies):
        bet_slots += [bookie] * bookie_limits[bookie]
        total_slots += bookie_limits[bookie]

    pairings = []
    for racer in range(racers):
        if len(bet_slots) == 0:
            break
        index = randint(0, len(bet_slots) - 1)
        pairings.append((racer, bet_slots[index]))
        del bet_slots[index]

    other_pairings = [
        (racer, bookie)
        for racer in range(racers)
        for bookie in range(bookies) if (racer, bookie) not in pairings]

    shuffle(pairings)
    shuffle(other_pairings)
    pairings += other_pairings
    values = [generate_fraction() for _ in range(bookies * racers)]
    zero_count = randint(0, len(other_pairings) // 2)

    # add some zeroes, but not too many
    for i in range(zero_count):
        values[i] = 0
    values.sort(reverse=True)

    # make sure the first values are strictly higher than the rest
    for i in range(racers):
        values[i] += Fraction(1, values[i].denominator)
    # re-sorting is needed, (see 5/8 > 1/2 but 6/8 < 2/2)
    values.sort(reverse=True)

    solution = 0
    if total_slots >= racers:
        solution = 1 / sum([1 / v for v in values[:racers]])

    bets = [[0] * bookies for _ in range(racers)]

    for ((racer, bookie), value) in zip(pairings, values):
        bets[racer][bookie] = value

    for racer_bets in bets:
        print(" ".join(map(str, racer_bets)))

    print(str.join(" ", [str(limit) for limit in bookie_limits]))
    print("# solution = {}".format(solution))


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

    seed(args.seed)

    number_of_inputs = args.count
    print(number_of_inputs)
    for i in range(number_of_inputs):
        generate_input(args.racers, args.bookies)


if __name__ == "__main__":
    main()
