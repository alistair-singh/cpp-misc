
template <class M, class A, class B, class F> M<B> bind(M<A> &&a, F fb) {
  return fb(a);
}

int main() { return 0; }
