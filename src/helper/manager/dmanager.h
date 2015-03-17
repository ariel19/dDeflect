#ifndef DMANAGER_H
#define DMANAGER_H

#include <QFile>
#include <QString>
#include <QList>
#include <QMap>
#include <helper/settings_parser/dsettings.h>
#include <helper/json_parser/djsonparser.h>


class DManager {
public:
  enum class AddingMethodType {
      OEP = 0,
      Thread,
      Trampoline,
      INIT,
      INIT_ARRAY,
      CTORS
  };

  class secured_file_info {
    QString file_name;
    AddingMethodType adding_method;
    QString dd_method;
    QString dd_handler;
    bool change_x;
    bool obfuscate;
    bool pack;
  public:
    secured_file_info() :
      change_x(true), obfuscate(false), pack(false) {}

    QString get_file_name() const;
    void set_file_name(const QString &value);
    AddingMethodType get_adding_method() const;
    void set_adding_method(const AddingMethodType &value);
    QString get_dd_method() const;
    void set_dd_method(const QString &value);
    QString get_dd_handler() const;
    void set_dd_handler(const QString &value);
    bool get_change_x() const;
    void set_change_x(bool value);
    bool get_obfuscate() const;
    void set_obfuscate(bool value);
    bool get_pack() const;
    void set_pack(bool value);
  };

  DManager();

  bool secure(const secured_file_info &sfi);

  void show_ddmethods() const;
  void show_ddhandlers() const;
  void show_adding_methods() const;
private:
  enum class OS {
    NONE,
    WIN,
    LIN
  };

  enum class Arch {
    X86,
    X64
  };

  typedef struct _arch_type {
    OS os;
    Arch arch;
  } arch_type;

  DSettings &settings;
  DJsonParser json_parser;

  // user information
  QMap<arch_type, QList<QString>> dd_methods;
  QMap<arch_type, QList<QString>> adding_methods;
  QMap<arch_type, QList<QString>> dd_handlers;

  // TODO: change variable name
  QMap<QString, QMap<arch_type, QList<QString>> *> mapper;

  template <typename RegistersType>
  bool __get_descriptions();

  bool __secure_elf(const QByteArray &data, const secured_file_info &sfi);
  bool __secure_pe(const QByteArray &data, const secured_file_info &sfi);

  template <typename RegistersType>
  bool __secure_elf(ELF *elf, const secured_file_info &sfi);

  template <typename RegistersType>
  bool __secure_pe(PEFile *pe, const secured_file_info &sfi);
};

#endif // DMANAGER_H
