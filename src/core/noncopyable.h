#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

namespace markly {
class Noncopyable {
protected:
  Noncopyable() = default;

  virtual ~Noncopyable() = default;

  Noncopyable(const Noncopyable &) = delete;
  Noncopyable &operator=(const Noncopyable &) = delete;
};
} // namespace markly

#endif // NONCOPYABLE_H
