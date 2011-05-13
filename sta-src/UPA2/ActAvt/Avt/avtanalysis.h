#ifndef AVTANALYSIS_H
#define AVTANALYSIS_H

#include <QObject>
#include <QMainWindow>
#include <QDialog>
#include "Eigen/Core"
#include "AVTACT.h"


USING_PART_OF_NAMESPACE_EIGEN

class QTabWidget;
class SpaceScenario;
class PropagatedScenario;
class input_class;

class avt_input_class
{
    public:
        QList<MatrixXd> treatment_mat;   //combined matrix 6x9
        QList<MatrixXd> fct_cmbntn;      //combination vector
        QList<MatrixXd> samples;         //sample group for the corresponding combination
};

class h_struct
{
    public:
        QList<MatrixXd> takecombination;
        QList<MatrixXd> takesamples;
};

class ok_struct
{
    public:
        QList<h_struct> h;
};

class SS_struct
{
     public:
        QList<VectorXd> SSwithinn;
        QList<VectorXd> SSbetween;
        QList<VectorXi> simple_combinations;
        QList<VectorXd> MS;
        QList<VectorXd> F_factor;
        int dof;
};

class AvtAnalysis
{
public:
    AvtAnalysis(input_class current_input);
    ~AvtAnalysis();
    VectorXd SStot;

    int combine_matrices();
    void print_matrix(MatrixXd matrix,FILE* wFile);        
    void print_matrix_int(MatrixXi matrix,FILE* wFile);
    void print_vector(VectorXd vector,FILE* wFile);
    void print_vector_int(VectorXi vector,FILE* wFile);
    int group_samples(MatrixXd samples,int facts);
    int save_propagation(QList<MatrixXd> cov, QList<MatrixXd> samples);
    int anova(int variation);
    avt_input_class get_combined_matrices(){return(m_matrix_combined);}
    VectorXd get_SStot(){return(SStot);}
    QList<SS_struct> get_SS(){return(m_SS);}
    VectorXd get_error(){return(m_error);}
    VectorXd get_errorMS(){return(m_errorMS);}
    int get_error_dof(){return(m_dof_err);}
    int get_n_factor(){return(n_factors);}
    void set_output_directory(QString directory_name) {m_directory=directory_name;}

private:
    MatrixXd m_check_mat;
    QList<MatrixXd> m_1param_mat,m_1param_vet;
    QList<MatrixXd> m_input;
    QList<avt_input_class> m_cov_fact;
    QList<avt_input_class> m_alltrets_allfacts, m_xtreats_xfacts;
    avt_input_class m_cov_tmp;
    avt_input_class m_treats1fact;
    avt_input_class m_matrix_combined;
    avt_input_class m_matrix_propagated;
    int n_factors, n_treat;
    QList<SS_struct> m_SS;
    VectorXd m_error;
    VectorXd m_errorMS;
    int m_dof_err;
    QString m_directory;

    int CreateOneParamMatrices();
    MatrixXi cycle(VectorXi fact_list, MatrixXi g);
    int factorial(int number);
    avt_input_class make_combination(int n_treat,int n_factors, avt_input_class f_mat_tbc,QList<avt_input_class> alltreats_allfacts,bool ok,bool print);
    VectorXd calculate_ss(MatrixXd mat);
};

#endif // AVTANALYSIS_H
