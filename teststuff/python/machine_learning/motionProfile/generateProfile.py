#!/usr/bin/env python3


if __name__=="__main__":

    import argparse

    parser = argparse.ArgumentParser(description="Optional app description")

    parser.add_argument("x3", type=float,
                        help="'saturation' coef/delta where values higher isn't affected")
    parser.add_argument("x2", type=float, nargs="?", const=1,
                        help="midrange coef/val between x1 and x3")
    parser.add_argument("x1", type=float, nargs="?", const=1,
                        help="breakpoint from 0-coef to scalars")

    args = parser.parse_args()


    print("Argument values:")
    print(args.x3)
    print(args.x2)
    print(args.x1)
    print("total:", args.x3+args.x2+args.x1)
