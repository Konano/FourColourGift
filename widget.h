#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTime>
#include <queue>
#include <set>

struct polygon {
    int a,b,c;
};
struct edge {
    int x, y;
    bool operator <(const edge &a) const { return x<a.x || (x==a.x && y<a.y); }
};

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

protected:
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *);

private:
    Ui::Widget *ui;
    int num, X[404], Y[404];
    std::priority_queue<std::pair<double,int>, std::vector<std::pair<double,int> >, std::greater<std::pair<double,int> > > q;
    int col[400];
    bool gaming;
    int C, Cnow;
    std::vector<int> l;
    int ltot;
    std::vector<int> v[400];
    int best;
    int cnt[5];
    std::vector<polygon> p;
    std::set<edge> e;

    bool inCircle(polygon poly, int point);
    double dis(int a, int b);
    void changeLabel();
    QString colorName(int c);
    Qt::GlobalColor colorQT(int c);
    void link(int a, int b);
    bool canLink(int a, int b);
    void paintEvent(QPaintEvent *);

    std::vector<int> st;
    int dfs_col[400], dfs_cnt[5];
    bool st_ed[400];
    QTime baseTime;

    void dfs(int o);
};

#endif // WIDGET_H
