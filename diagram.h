#ifndef DIAGRAM_H
#define DIAGRAM_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHash>
#include <QStyleFactory>
#include "connectingline.h"
#include "widget.h"
#include <QLabel>
#include "myconstants.h"
namespace Ui {
class Diagram;
}
//class Widget;
class Diagram : public QPushButton
{
    Q_OBJECT

public:
    explicit Diagram(QWidget *parent = nullptr,QWidget *mainWidget = nullptr ,QString name="");
    ~Diagram();
   // QPushButton* button;
    Ui::Diagram *ui;
    QWidget *mainWidget;
    QWidget *parent;
    QString name;
    int lineIndex = 0;
    bool adicionandoPontos = false;
    bool isActive () const;
    bool isOpen() const;
    void setPrerequisites( QVector<Diagram*>* prerequisites);
    void setCargaHoraria(int horasAula, bool obrigatoria = true);
    void setPreCH(int horasAula);
    int cargaHoraria = 0;      // horas-aula (H/A) da disciplina
    bool obrigatoria = true;   // optativas não contam para o "Pré CH"
    int preCH = 0;             // H/A obrigatórias concluídas exigidas (0 = nenhuma)
    int faltamHoras() const;   // quanto falta para atender preCH (0 = atendido)
    // grupos temáticos (informatica, controle, automacao, mecanica, eletrica,
    // fisica_calculo); a caixa é pintada com a cor do grupo, e com dois grupos
    // é dividida na diagonal
    QStringList grupos;
    void setGrupos(const QStringList& grupos);
    static QString corDoGrupo(const QString& grupo);
    static QString nomeDoGrupo(const QString& grupo);
    enum Estado { Bloqueada, Disponivel, Concluida };
    void aplicarEstado(Estado estado);
    void reaplicar() { aplicarEstado(estado); }
    // "Colorir por": false = situação (verde/azul/cinza), true = grupo.
    // grupoFoco (só no modo grupo): destaca um grupo e apaga o resto
    static bool modoGrupo;
    static QString grupoFoco;
    void addPointToLine(QPoint point);
    void addNewLine();
    void incIndex();
    void decIndex();
    QList<QPoint>* getCurrentLine();
    bool operator==(const Diagram &other) const;
    uint myQHash(const Diagram &key);
     QVector<Diagram*>* prerequisites= nullptr;
     QVector<ConnectingLine*> lines;
signals:
    void checkPrerequisites();
public slots:
    void setActive_slot();
    void setActive(bool set);
    void buildLines();
    void paintDiagramColor(QString color);
    void paintDiagramLines(const QColor& color);
protected:

   // void paintEvent(QPaintEvent *e) override;

private:
    bool active=false;
    Estado estado = Disponivel;
    void atualizarTexto();
};

#endif // DIAGRAM_H
