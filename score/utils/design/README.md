# Detailed Design

`lib/utils` imposes a grouping function on small functionalities that can be reused in other parts of our code, but does
not really match into one of our common libraries.

Overall we would like to get rid of `lib/utils` since it is quite unspecific and it is unclear from the name what is
behind. But there will be always the need for a "last resort" collection facility.
That's why the detailed design does not really cover a static or dynamic design that illustrates the dependency between
the single classes of `lib/utils`, but rather how single classes operate.

## PimplPtr

Not yet decided if deprecated or not, since it is only used at one location in our code base and its unclear if a big
benefit exists from it.

## PayloadValidation

Only one function that is standalone as such.

## StringHash

Helper to calculate string hash.

## TimeConversion

Different functions for time conversion not available in the standard library.

## ScopedOperation

Is a small helper class that follows the ideas of https://en.cppreference.com/w/cpp/experimental/scope_exit where you
the class just stores a callback and executes that when the class gets destructed.
