#ifndef DADDINGMETHODS_H
#define DADDINGMETHODS_H

#include <QString>
#include <QMap>
#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QFile>
#include <core/file_types/codedefines.h>
#include <core/file_types/binaryfile.h>
#include <core/file_types/elffile.h>

template <typename RegistersType>
class Wrapper;

#define mnemonic_stringify(mnemonic) \
    QString((std::string(#mnemonic).substr(std::string(#mnemonic).find_last_of(':') != std::string::npos ? \
    std::string(#mnemonic).find_last_of(':') + 1 : 0)).c_str()).toLower()

#define enum_stringify(enum_val) \
    QString((std::string(#enum_val).substr(std::string(#enum_val).find_last_of(':') != std::string::npos ? \
    std::string(#enum_val).find_last_of(':') + 1 : 0)).c_str()).toLower()

#define instruction_stringify(instruction) \
    QString((std::string(#instruction).substr(std::string(#instruction).find_last_of(':') != std::string::npos ? \
    std::string(#instruction).find_last_of(':') + 1 : 0)).c_str()).toLower()

/**
 * @brief Klasa bazowa dla dodawania kodu dla określonych typów plików.
 */
template <typename RegistersType>
class DAddingMethods {
public:

    /**
     * @brief Typ kompilowanego pliku assembly.
     */
    enum class ArchitectureType {
        BITS32,
        BITS64
    };

    /**
     * @brief Obsługiwane systemy operacyjne.
     */
    enum class SystemType {
        Windows,
        Linux
    };

    /**
     * @brief Typy możliwości wstrzyknięcia kodu.
     */
    enum class CallingMethod {
        OEP = 0,
        Thread,
        Trampoline,
        INIT,
        INIT_ARRAY,
        CTORS,
        TLS
    };

    /**
     * @brief Stopień kompresji UPX.
     */
    enum class CompressionLevel {
        L1 = 1,
        L2,
        L3,
        L4,
        L5,
        L6,
        L7,
        L8,
        L9,
        BEST
    };

    /**
     * @brief Dodatkowe opcje kompresji UPX.
     */
    enum class CompressionOptions {
        Default,
        Brute,
        Ultra
    };

    /**
     * @brief Klasa opisująca metodę wstrzykiwania kodu.
     */
    class InjectDescription {
    public:
        CallingMethod cm;
        Wrapper<RegistersType> *adding_method;
        // QString saved_fname;
        bool change_x_only;
    };

    /**
     * @brief Konstruktor.
     */
    DAddingMethods(BinaryFile *f);

    /**
     * @brief Metoda dodająca wybrane metody wykrywania debuggerów do pliku binarnego.
     * @param descs Opisy metod wykrywania debuggerów.
     * @return True w przypadku sukcesu, False w innych przypadkach.
     */
    virtual bool secure(const QList<typename DAddingMethods<RegistersType>::InjectDescription*> &descs) = 0;

    /**
     * @brief Metoda pakująca plik binarny.
     * @param file_path Ścieżka do pliku.
     * @param level Poziom pakowania UPX.
     * @param opt Dodatkowe opcje pakowania.
     * @return True w przypadku sukcesu, False w innych przypadkach.
     */
    static bool pack(QString file_path, CompressionLevel level = CompressionLevel::BEST, CompressionOptions opt = CompressionOptions::Default);

protected:
    /**
     * @brief Plik binarny.
     */
    BinaryFile *file;

    /**
     * @brief Mapa zawierająca tekstowe opisy architektur.
     */
    QMap<ArchitectureType, QString> arch_type;

    /**
     * @brief Generator liczb losowych.
     */
    std::default_random_engine r_gen;

public:
    /**
     * @brief Mapa konwertująca ciągi znaków na CallingMethod.
     */
    static const QMap<QString, CallingMethod> callingMethods;
};

/**
 * @brief Klasa generowania źródłowego kodu assembly.
 */
class AsmCodeGenerator {
    static const QMap<Registers_x86, QString> regs_x86;
    static const QMap<Registers_x64, QString> regs_x64;

    /**
     * @brief Typy instrukcji assembly.
     */
    enum class Instructions {
        POP,
        PUSH,
        MOV,
        JMP,
        CALL
    };

    /**
     * @brief reprezentacja stringowa instrukcji.
     */
    static const QMap<Instructions, QString> instructions;

public:
    /**
     * @brief Konstruktor.
     */
    AsmCodeGenerator() { }

    /**
     * @brief Generowanie kodu odkładania rejestru na stosie.
     * @param reg rejestr.
     * @return Kod.
     */
    template <typename RegistersType>
    static QString push_regs(const RegistersType reg);

    /**
     * @brief Generowanie kodu odkładania rejestru na stosie.
     * @param regs lista rejestrów.
     * @return Kod.
     */
    template <typename RegistersType>
    static QString push_regs(const QList<RegistersType> &regs);

    /**
     * @brief Generowanie kodu pobierania rejestru ze stosu.
     * @param reg rejestr.
     * @return Kod.
     */
    template <typename RegistersType>
    static QString pop_regs(const RegistersType reg);

    /**
     * @brief Generowanie kodu pobierania rejestru ze stosu.
     * @param regs lista rejestrów.
     * @return Kod.
     */
    template <typename RegistersType>
    static QString pop_regs(const QList<RegistersType> &regs);

    /**
     * @brief Generowanie kodu, zachowującego flagi procesora na stosie.
     * @return Kod.
     */
    template <typename RegistersType>
    static QString save_flags();

    /**
     * @brief Generowanie kodu, pobierającego flagi procesora ze stosu.
     * @return Kod.
     */
    template <typename RegistersType>
    static QString restore_flags();

    /**
     * @brief Generowanie kodu, ładującego pewną stałą do określonego rejestru.
     * @param reg rejestr.
     * @param value stała.
     * @return Kod.
     */
    template <typename RegistersType>
    static QString mov_reg_const(const RegistersType reg, Elf64_Addr value);

    /**
     * @brief Generowanie kodu, skaczącego pod zawartość określonego rejestru.
     * @param reg rejestr.
     * @return Kod.
     */
    template <typename RegistersType>
    static QString jmp_reg(const RegistersType reg);

    /**
     * @brief Generowanie kodu, skaczącego relatywnie o wartość offsetu.
     * @param addr offset.
     * @return Kod.
     */
    template <typename RegistersType>
    static QString jmp_rel(int32_t addr);

    /**
     * @brief Generowanie reprezentacji stringowej określonego rejestru.
     * @param reg rejestr.
     * @return Reprezentacja stringowa rejestru.
     */
    template <typename RegistersType>
    static QString get_reg(const RegistersType reg);

    /**
     * @brief Generowanie kodu, wołającego adres w rejestrze.
     * @param reg rejestr.
     * @return Kod.
     */
    template <typename RegistersType>
    static QString call_reg(const RegistersType reg);

    /**
     * @brief Generowanie kodu, wołającego relatywnie adres o wartość offsetu.
     * @param value offset.
     * @return Kod.
     */
    static QString call_const(Elf64_Addr value) {
        return QString("%1 %2\n").arg(instructions[Instructions::CALL], QString::number(value));
    }
};

template <typename RegistersType>
QString AsmCodeGenerator::push_regs(const RegistersType reg) {
    if (std::is_same<RegistersType, Registers_x86>::value) {
        if (static_cast<Registers_x86>(reg) == Registers_x86::None)
            return QString();
        if (static_cast<Registers_x86>(reg) == Registers_x86::All)
            return QString("pushad\n");
        return QString("%1 %2\n").arg(instructions[Instructions::PUSH], regs_x86[static_cast<Registers_x86>(reg)]);
    }
    else if (std::is_same<RegistersType, Registers_x64>::value) {
        if (static_cast<Registers_x64>(reg) == Registers_x64::None)
            return QString();
        if (static_cast<Registers_x64>(reg) == Registers_x64::All) {
            QString gen_code;
            QList<Registers_x64> regs;
            foreach (Registers_x64 r, regs)
                regs.append(r);

            regs.removeAll(Registers_x64::None);
            regs.removeAll(Registers_x64::All);

            foreach (Registers_x64 r, regs)
                gen_code.append(QString("%1 %2\n").arg(instructions[Instructions::PUSH], regs_x64[r]));

            return gen_code;
        }

        return QString("%1 %2\n").arg(instructions[Instructions::PUSH], regs_x64[static_cast<Registers_x64>(reg)]);
    }
}

template <typename RegistersType>
QString AsmCodeGenerator::push_regs(const QList<RegistersType> &regs) {
    QString gen_code;
    foreach (RegistersType r, regs)
        gen_code.append(AsmCodeGenerator::push_regs<RegistersType>(r));

    return gen_code;
}

template <typename RegistersType>
QString AsmCodeGenerator::pop_regs(const RegistersType reg) {

    if (std::is_same<RegistersType, Registers_x86>::value) {
        if (static_cast<Registers_x86>(reg) == Registers_x86::None)
            return QString();
        if (static_cast<Registers_x86>(reg) == Registers_x86::All)
            return QString("popad\n");
        return QString("%1 %2\n").arg(instructions[Instructions::POP], regs_x86[static_cast<Registers_x86>(reg)]);
    }
    else if (std::is_same<RegistersType, Registers_x64>::value) {
        if (static_cast<Registers_x64>(reg) == Registers_x64::None)
            return QString();
        if (static_cast<Registers_x64>(reg) == Registers_x64::All) {
            QString gen_code;
            QList<Registers_x64> regs;
            foreach (Registers_x64 r, regs)
                regs.append(r);

            regs.removeAll(Registers_x64::None);
            regs.removeAll(Registers_x64::All);

            foreach (Registers_x64 r, regs)
                gen_code.append(QString("%1 %2\n").arg(instructions[Instructions::POP], regs_x64[r]));

            return gen_code;
        }

        return QString("%1 %2\n").arg(instructions[Instructions::POP], regs_x64[static_cast<Registers_x64>(reg)]);
    }
}

template <typename RegistersType>
QString AsmCodeGenerator::pop_regs(const QList<RegistersType> &regs) {
    QString gen_code;
    foreach (RegistersType r, regs)
        gen_code.append(AsmCodeGenerator::pop_regs<RegistersType>(r));

    return gen_code;
}

template <typename RegistersType>
QString AsmCodeGenerator::save_flags() {
    return std::is_same<RegistersType, Registers_x86>::value ?
                QString("pushf\n") :
                std::is_same<RegistersType, Registers_x64>::value ?
                QString("pushfq\n") : QString();
}

template <typename RegistersType>
QString AsmCodeGenerator::restore_flags() {
    return std::is_same<RegistersType, Registers_x86>::value ?
                QString("popf\n") :
                std::is_same<RegistersType, Registers_x64>::value ?
                QString("popfq\n") : QString();
}

template <typename RegistersType>
QString AsmCodeGenerator::mov_reg_const(const RegistersType reg, Elf64_Addr value) {
    QString qreg = std::is_same<RegistersType, Registers_x86>::value ?
                regs_x86[static_cast<Registers_x86>(reg)] :
        std::is_same<RegistersType, Registers_x64>::value ?
        regs_x64[static_cast<Registers_x64>(reg)] : "xxx";
    return QString("%1 %2, %3\n").arg(instructions[Instructions::MOV], qreg, QString::number(value));
}

template <typename RegistersType>
QString AsmCodeGenerator::jmp_reg(const RegistersType reg) {
    QString qreg = std::is_same<RegistersType, Registers_x86>::value ?
                regs_x86[static_cast<Registers_x86>(reg)] :
        std::is_same<RegistersType, Registers_x64>::value ?
        regs_x64[static_cast<Registers_x64>(reg)] : "xxx";
    return QString("%1 %2\n").arg(instructions[Instructions::JMP], qreg);
}

template <typename RegistersType>
QString AsmCodeGenerator::jmp_rel(int32_t rel_addr) {
    return QString("%1 %2").arg(instructions[Instructions::JMP], QString::number(rel_addr));
}

template <typename RegistersType>
QString AsmCodeGenerator::get_reg(const RegistersType reg) {
    return std::is_same<RegistersType, Registers_x86>::value ?
                regs_x86[static_cast<Registers_x86>(reg)] :
        std::is_same<RegistersType, Registers_x64>::value ?
                    regs_x64[static_cast<Registers_x64>(reg)] : "xxx";
    }

template <typename RegistersType>
QString AsmCodeGenerator::call_reg(const RegistersType reg) {
    QString qreg = std::is_same<RegistersType, Registers_x86>::value ?
    regs_x86[static_cast<Registers_x86>(reg)] :
    std::is_same<RegistersType, Registers_x64>::value ?
    regs_x64[static_cast<Registers_x64>(reg)] : "xxx";
    return QString("%1 %2\n").arg(instructions[Instructions::CALL], qreg);
}

template <typename RegistersType>
class OEPWrapper;
template <typename RegistersType>
class TrampolineWrapper;
template <typename RegistersType>
class ThreadWrapper;

/**
 * @brief Klasa, odpowiadająca za reprezentacje kodu opakowania.
 */
template <typename RegistersType>
class Wrapper : public QObject{
public:
    /**
     * @brief Typy opakowań.
     */
    enum class WrapperType {
        Handler,
        Method,
        Helper,
        ThreadWrapper,
        OepWrapper,
        TrampolineWrapper
    };

    /**
     * @brief Nazwa metody.
     */
    QString name;

    /**
     * @brief Opis metody.
     */
    QString description;

    /**
     * @brief Architektura metody.
     */
    typename DAddingMethods<RegistersType>::ArchitectureType arch_type;

    /**
     * @brief System docelowy metody.
     */
    typename DAddingMethods<RegistersType>::SystemType system_type;

    /**
     * @brief Typ opakowania.
     */
    WrapperType wrapper_type;

    /**
     * @brief Lista dozwolonych typów głównych opakowań.
     */
    QList<typename DAddingMethods<RegistersType>::CallingMethod> allowed_methods;

    /**
     * @brief Lista używanych rejestrów.
     */
    QList<RegistersType> used_regs;

    /**
     * @brief Parametry dynamiczne.
     */
    QMap<RegistersType, QString> dynamic_params;

    /**
     * @brief Parametry statyczne.
     */
    QMap<QString, QString> static_params;

    /**
     * @brief Rejestr, w którym jest zwracana wartość działania metody.
     */
    RegistersType ret;

    /**
     * @brief Kod metody.
     */
    QByteArray code;

    /**
     * @brief Handler w razie detekcji debuggera.
     */
    Wrapper *detect_handler;

    /**
     * @brief Flaga, określająca czy zmieniany segment musi mieć prawa dostępu R^W^X.
     */
    bool only_rwx;

    /**
     * @brief Flaga, określająca czy podany plik może podlegać zaciemnianiu.
     */
    bool obfuscation;

    /**
     * @brief Destruktor.
     */
    virtual ~Wrapper() {}

    /**
     * @brief Konstruktor kopiujący.
     * @param w instancja Wrapper.
     */
    Wrapper(const Wrapper& w){
        // TODO: WTF?????? should be changed
        name = w.name;
        description = w.description;
        wrapper_type = w.wrapper_type;
        allowed_methods = w.allowed_methods;
        used_regs = w.used_regs;
        dynamic_params = w.dynamic_params;
        static_params = w.static_params;
        ret = w.ret;
        code = w.code;
        detect_handler = nullptr;
        only_rwx = w.only_rwx;
        obfuscation = w.obfuscation;
    }

    /**
     * @brief Konstruktor.
     */
    Wrapper(){ detect_handler = nullptr; }

    /**
     * @brief Reprezentacja stringów typami opakowań.
     */
    static const QMap<QString, WrapperType> wrapperTypes;

    /**
     * @brief Metoda kopiująca wrapper wraz z typem
     * @param w Wrapper
     * @return Nowy wrapper
     */
    static Wrapper *copy(Wrapper *w) {
        switch(w->wrapper_type)
        {
        case WrapperType::OepWrapper:
            return new OEPWrapper<RegistersType>(*dynamic_cast<OEPWrapper<RegistersType>*>(w));

        case WrapperType::ThreadWrapper:
            return new ThreadWrapper<RegistersType>(*dynamic_cast<ThreadWrapper<RegistersType>*>(w));

        case WrapperType::TrampolineWrapper:
            return new TrampolineWrapper<RegistersType>(*dynamic_cast<TrampolineWrapper<RegistersType>*>(w));

        default:
            return new Wrapper(*w);
        }
    }

    /**
     * @brief read this zostaje wczytany z pliku json.
     * @param json objekt json'a z którego czytamy.
     */
    virtual bool read(const QJsonObject & json){

        // name
        name = json["name"].toString();

        // description
        description = json["description"].toString();

        // arch_type
        QRegExp x86arch("^(win|lin)_x86$");
        QRegExp x64arch("^(win|lin)_x64$");
        QString arch_str = json["architecture"].toString();
        if(arch_str.contains(x86arch))
            arch_type = DAddingMethods<RegistersType>::ArchitectureType::BITS32;
        else if(arch_str.contains(x64arch))
            arch_type = DAddingMethods<RegistersType>::ArchitectureType::BITS64;
        else
            return false;

        // system_type
        QRegExp system_win("^win_(x86|x64)$");
        QRegExp system_lin("^lin_(x86|x64)$");
        QString system_str = json["architecture"].toString();
        if(system_str.contains(system_win))
            system_type = DAddingMethods<RegistersType>::SystemType::Windows;
        else if(system_str.contains(system_lin))
            system_type = DAddingMethods<RegistersType>::SystemType::Linux;
        else
            return false;

        // wrapper_type
        wrapper_type = wrapperTypes[json["type"].toString()];

        // allowed_methods
        QJsonArray allowedMethods = json["methods"].toArray();
        foreach(auto m, allowedMethods)
            allowed_methods.append(DAddingMethods<RegistersType>::callingMethods[m.toString()]);

        // used_regs
        QJsonArray regList = json["used_registers"].toArray();
        foreach(auto r, regList)
            used_regs.append(registerTypes[r.toString()]);

        // static_params
        if(system_type == DAddingMethods<RegistersType>::SystemType::Linux)
        {
            QJsonArray temp = json["parameters"].toArray();

            foreach(auto key, temp) {
                if(key.toObject().keys().length() != 1)
                    return false;
                QString k = key.toObject().keys()[0];
                static_params.insert(k, key.toObject()[k].toString());
            }
        }

        // dynamic_params
        if(system_type == DAddingMethods<RegistersType>::SystemType::Windows)
        {
            QJsonArray temp = json["parameters"].toArray();

            foreach(auto key, temp) {
                if(key.toObject().keys().length() != 1)
                    return false;
                QString k = key.toObject().keys()[0];
                dynamic_params.insert(registerTypes[k], key.toObject()[k].toString());
            }
        }

        // ret
        ret = registerTypes[json["returns"].toString()];

        // code
        QFile codeFile(json["path"].toString());
        if(!codeFile.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;

        code = codeFile.readAll();
        codeFile.close();

        // detect_handler
        detect_handler = nullptr;

        // methods that are only suitable for rwx segments
        only_rwx = false;
        if (json["only_rwx"].toString() == QString("yes"))
            only_rwx = true;

        // is obfuscation eligable
        obfuscation = true;
        if (json["obfuscation"].toString() == QString("no"))
            obfuscation = false;

        return true;
    }

private:
    /**
     * @brief Reprezentacja stringów typami rejestrów.
     */
    static const QMap<QString,RegistersType> registerTypes;

    /**
     * @brief write zapisujemy obiekt this do pliku json
     * @param json obiekt do którego zapisujemy
     */
    bool write(QJsonObject &json) const{

        //            // used_regs
        //            QJsonArray array;

        //            const char** tab = ( std::is_same<RegistersType, Registers_x64>::value ) ? Registers_x64_names : Registers_x86_names;

        //            for(int i = 0; i< used_regs.size(); ++i){
        //                array.append(QJsonValue(QString( tab[static_cast<int>(used_regs[i])])));
        //            }
        //            json["used_regs"]=array;

        //            // params
        //            QJsonObject par;
        //            foreach(QString key, params.keys()){
        //                par.insert(key, QJsonValue(params.value(key)));
        //            }
        //            json["params"] = par;

        //            // ret

        //            json["ret"] = QString(tab[static_cast<int>(ret)]);

        //            // code

        //            QString codePath("./codeFile.cpp");
        //            QFile codeFile(codePath);
        //            QFileInfo fi(codeFile);
        //            codeFile.open(QIODevice::WriteOnly | QIODevice::Text);
        //            QTextStream in(&codeFile);
        //            in<<code;

        //            json["path_to_method"] = fi.absoluteFilePath();

        return false;
    }
};

/**
 * @brief Klasa reprezentująca opakowanie dla tworzenia nowego wątku.
 */
template <typename RegistersType>
class ThreadWrapper : public Wrapper<RegistersType> {
public:
    QList<Wrapper<RegistersType>*> thread_actions;
    uint16_t sleep_time;

    ThreadWrapper() { sleep_time = 5; }
    ThreadWrapper(const ThreadWrapper& w) : Wrapper<RegistersType>(w) {
        sleep_time = 5;
    }

    virtual bool read(const QJsonObject & json) override {
        sleep_time = 5;
        return Wrapper<RegistersType>::read(json);
    }
};

/**
 * @brief Klasa reprezentująca opakowanie dla tworzenia nowego punktu wejściowego.
 */
template<typename RegistersType>
class OEPWrapper : public Wrapper<RegistersType> {
public:
    Wrapper<RegistersType> *oep_action;

    OEPWrapper() { oep_action = nullptr; }
    OEPWrapper(const OEPWrapper& w) : Wrapper<RegistersType>(w) {
        oep_action = nullptr;
    }

    virtual bool read(const QJsonObject & json) override {
        oep_action = nullptr;
        return Wrapper<RegistersType>::read(json);
    }
};

/**
 * @brief Klasa reprezentująca opakowanie dla tworzenia tramplin w funkcjach bibliotecznych.
 */
template<typename RegistersType>
class TrampolineWrapper : public Wrapper<RegistersType> {
public:
    Wrapper<RegistersType> *tramp_action;

    TrampolineWrapper() { tramp_action = nullptr; }
    TrampolineWrapper(const TrampolineWrapper& w) : Wrapper<RegistersType>(w) {
        tramp_action = nullptr;
    }

    virtual bool read(const QJsonObject & json) override {
        tramp_action = nullptr;
        return Wrapper<RegistersType>::read(json);
    }
};

#endif // DADDINGMETHODS_H
