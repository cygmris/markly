#ifndef IVERSIONCONTROLLER_H
#define IVERSIONCONTROLLER_H

#include <QString>

namespace markly {
// Version control interface for a notebook. v3.20 of VNote only ships a no-op
// (dummy) controller; the vx_notebook.json "version_controller" field is
// "dummy.vnotex". A real controller can replace this in a future spec.
class IVersionController {
public:
  virtual ~IVersionController() = default;

  virtual QString getName() const = 0;

  // Lifecycle hooks invoked by the notebook on node changes (no-op for dummy).
  virtual void addFile(const QString &p_path) = 0;
  virtual void removeFile(const QString &p_path) = 0;
  virtual void renameFile(const QString &p_path, const QString &p_newPath) = 0;
};

// No-op version controller.
class DummyVersionController : public IVersionController {
public:
  QString getName() const override { return QStringLiteral("dummy.vnotex"); }

  void addFile(const QString &) override {}
  void removeFile(const QString &) override {}
  void renameFile(const QString &, const QString &) override {}
};
} // namespace markly

#endif // IVERSIONCONTROLLER_H
