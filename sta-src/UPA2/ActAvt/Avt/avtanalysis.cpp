#include "avtanalysis.h"
#include <QObject>
#include <QMainWindow>
#include <QDialog>
#include "Eigen/Core"
#include <cmath>
#include <stdio.h>


USING_PART_OF_NAMESPACE_EIGEN



AvtAnalysis::AvtAnalysis(input_class current_input)
{
    // Define check matrix
    m_check_mat.setZero(6,9);
    m_check_mat.block(0,0,6,3) = current_input.distribution_vect1.at(0);
    m_check_mat.block(0,3,6,6) = current_input.covariance.at(0);

    // Define input matrix list
    MatrixXd mat_tmp;
    m_input.clear();
    for (int i=1;i<current_input.treat_num;i++)
    {
        mat_tmp.setZero(6,9);
        mat_tmp.block(0,0,6,3) = current_input.distribution_vect1.at(i);
        mat_tmp.block(0,3,6,6) = current_input.covariance.at(i);
        m_input.append(mat_tmp);
    }
}

int AvtAnalysis::CreateOneParamMatrices()
{
    // create QList MatrixXd m_OneParamMat  ----------
    MatrixXd Mf;
    m_1param_mat.clear();

    int k=0;
    for (int i=0;i<m_check_mat.rows();i++)
    {
        for (int j=0;j<m_check_mat.cols();j++)
        {
            Mf.setZero(6,9);
            if (m_check_mat(i,j)==1)
            {
                Mf(i,j)=1;
                m_1param_mat.append(Mf);
                k=k+1;
            }
        }
    }
}


int AvtAnalysis::combine_matrices()
{
    MatrixXd m_print;

    n_treat = m_input.count();    // number of tretments
    n_factors=m_check_mat.sum();         // number of varied factors

    for (int l=0;l<m_input.count();l++)

    CreateOneParamMatrices();
    // At least one factor must be selected
    if (m_1param_mat.count()==0)
    {
        return(0);
    }

    m_cov_fact.clear();
    m_cov_tmp.treatment_mat.clear();
    m_cov_tmp.fct_cmbntn.clear();
    MatrixXd Mc,Vc;
    Vc.setZero(1,n_factors);

    for (int i=0;i<n_factors;i++) //  %% cycle on n_factors
    {
        for (int j=0;j<n_treat;j++) //   % cycle on n treat
        {
            // %% this goes to be a Qlist of a Qlist
            Vc.setZero(1,n_factors);
            Mc=m_1param_mat.at(i).cwise()* m_input.at(j);
            Vc(0,i)=j+1;
            m_cov_tmp.treatment_mat.append(Mc);
            m_cov_tmp.fct_cmbntn.append(Vc);
        }
        m_cov_fact.append(m_cov_tmp);
        m_cov_tmp.treatment_mat.clear();
        m_cov_tmp.fct_cmbntn.clear();
    }

    avt_input_class f_mat_tbc;

    f_mat_tbc = m_cov_fact.at(0);      // %% first matrices to be combined

    f_mat_tbc = make_combination(n_treat,n_factors,f_mat_tbc,m_cov_fact,true,false);
    
    MatrixXd fixed_mat;
    fixed_mat.setZero(m_check_mat.rows(),m_check_mat.cols());


    for (int i=0;i<m_check_mat.rows();i++)
        for (int j=0;j<m_check_mat.cols();j++)
            fixed_mat(i,j)=m_input.at(0)(i,j)*(m_check_mat(i,j)-1)*(-1);

    MatrixXd temp_mat,temp_vet;
    temp_mat.setZero(m_check_mat.rows(),m_check_mat.cols());
    temp_vet.setZero(1,n_factors);
    for (int y=0;y< f_mat_tbc.treatment_mat.count();y++)
    {
        temp_mat=f_mat_tbc.treatment_mat.at(y);
        temp_vet=f_mat_tbc.fct_cmbntn.at(y);
        temp_mat=temp_mat+fixed_mat;

        // Symmetric covariance matrix---------------------------------------
        for (int i=0; i<temp_mat.rows();i++)
             for (int j=3; j<temp_mat.cols();j++)
                 temp_mat(j-3,i+3)=temp_mat(i,j);

        m_matrix_combined.treatment_mat.append(temp_mat);
        m_matrix_combined.fct_cmbntn.append(temp_vet);
    }
    // Combination process succesfully terminated
    return(1);
}



int AvtAnalysis::group_samples(MatrixXd sample,int facts)
{
    int n_row=sample.rows();
    int n_col=sample.cols();
    MatrixXd treat_num,one_param_vet;
    treat_num.setZero(1,facts);
    one_param_vet.setZero(1,facts);

    //Collect the number of treatments for each factors
    for (int i=0;i<n_row;i++)
    {
        for (int j=0;j<facts;j++)
        {
            if(treat_num(0,j)<sample(i,6+j))
                treat_num(0,j)=sample(i,6+j);
        }
    }

    //Make the list of treatment for each factor (Nfact x Ntreat) vectors
    int number_of_treat=treat_num(0,0);
    for  (int i=1; i<treat_num.cols();i++)
    {
        if (treat_num(0,i)>number_of_treat)
        {
            number_of_treat=treat_num(0,i);
            return(2);
        }
    }

    for  (int i=0;i<facts;i++)
    {
        for  (int j=0;j<number_of_treat;j++)
        {
            one_param_vet(0,i)=j+1;
            m_treats1fact.fct_cmbntn.append(one_param_vet);
            one_param_vet.setZero(1,facts);
        }
        m_alltrets_allfacts.append(m_treats1fact);
        m_treats1fact.fct_cmbntn.clear();
    }

    //Make the combination of treatment for all factors (Ntreat^Nfact) vectors
    avt_input_class f_vet_tbc;
    f_vet_tbc =m_alltrets_allfacts.at(0);      // %% first matrices to be combined


    f_vet_tbc = make_combination(number_of_treat,facts,f_vet_tbc,m_alltrets_allfacts,false,false);

    // Divide the samples according to the treatment-factor combinations
    n_row=sample.rows();
    MatrixXd grouped_sample;

    QList <MatrixXd> samplegroup;

    int ok, count;
    for (int k=0;k<f_vet_tbc.fct_cmbntn.count();k++)
    {
        for (int i=0;i<n_row;i++)
        {
            ok=0;
            for (int j=6;j<facts+6;j++)
            {
                if (sample(i,j)==f_vet_tbc.fct_cmbntn.at(k)(0,j-6))
                {
                    ok=ok+1;
                }
                else
                {
                    ok=0;
                }
            }
            if (ok==facts)
                samplegroup.append(sample.row(i));
        }
        if (samplegroup.count()>0)
        {

            grouped_sample.setZero(samplegroup.count(),6);
            for (int i=0;i<samplegroup.count();i++)
            {
                for (int j=0; j<6; j++)
                    grouped_sample(i,j)=samplegroup.at(i)(0,j);
            }
            f_vet_tbc.samples.append(grouped_sample);
        }
        else
        {
            return(0);
        }
        samplegroup.clear();
    }

    this->m_matrix_combined=f_vet_tbc;

    // Combination process succesfully terminated
    return(1);
}


avt_input_class AvtAnalysis::make_combination(int n_treat,int n_fact, avt_input_class f_mat_tbc,QList<avt_input_class> alltreats_allfacts,bool ok,bool print)
{
    MatrixXd f_tmp_mat,f_tmp_vet;
    QList<MatrixXd> f_tmp_mat_list,f_tmp_vet_list;
    int n_treat_z=n_treat;
    int z;
    int j;

    for (int i=0;i<n_fact-1;i++)
    {
         z=0;
         j=0;
         f_tmp_mat_list.clear();
         f_tmp_vet_list.clear();
         while (j<n_treat_z)
         {
             for (int k=0;k<n_treat;k++)
             {
             f_tmp_mat = f_mat_tbc.fct_cmbntn.at(j) + alltreats_allfacts.at(i+1).fct_cmbntn.at(k);
             f_tmp_mat_list.append(f_tmp_mat);

             if (ok)
             {
                f_tmp_vet = f_mat_tbc.treatment_mat.at(j) + alltreats_allfacts.at(i+1).treatment_mat.at(k);
                f_tmp_vet_list.append(f_tmp_vet);
             }
             z++;
             }
             j++;
        }
        f_mat_tbc.fct_cmbntn.clear();
        f_mat_tbc.fct_cmbntn.append(f_tmp_mat_list);
        if (ok)
        {
            f_mat_tbc.treatment_mat.clear();
            f_mat_tbc.treatment_mat.append(f_tmp_vet_list);
        }
        n_treat_z=z;
    }
    return (f_mat_tbc);
}

int AvtAnalysis::anova(int variation)
{
    // from Matrix propagated--------------------

    m_SS.clear();
    MatrixXd sample_vect;

    QString variation_ts;
    variation_ts.setNum(variation,10);
    int n_combination = m_matrix_propagated.fct_cmbntn.count();
    int n_samples = m_matrix_propagated.samples.at(0).rows();
    int length_all_samples = n_combination*n_samples;

    int n_col=6;
    sample_vect.setZero(length_all_samples,n_col);

    // calculate SStot ------------------------------------------------
    for (int j=0;j<n_combination;j++)
       for (int i=0;i<n_samples;i++)
           {
           sample_vect.block(j*n_samples,0,n_samples,n_col) = m_matrix_propagated.samples.at(j);
           }

    SStot = calculate_ss(sample_vect);

    // create vector factor list  -------------------------------------
    VectorXi fact_list(n_factors);
    for (int i=0; i<n_factors;i++)
        fact_list(i)=i;

    MatrixXi g;
    QList<MatrixXi> position;
    position.clear();

    // calculate combinations of one factor ---------------------------
    g.setZero(n_factors,1);

    for (int i=0;i<n_factors;i++)
        g(i,0)=i;
    position.append(g);

    // calculate combinations from 2 to (n_factors-1) factors
    MatrixXi tmp;
    for (int x=0;x<n_factors-1;x++)
    {
        tmp=cycle(fact_list,position.at(x));
        position.append(tmp);
    }

    int max_pos = position.count();
    MatrixXd g_comb, covaried, f_tmp, f_mat_tbc;
    MatrixXd one_param_vet;

    QList<ok_struct> ok;
    ok_struct ok_tmp;
    h_struct h_tmp;
    QList<h_struct> h_list;

    for (int g_num=0;g_num<max_pos;g_num++)
    {

        int n_comb_sempl= position.at(g_num).rows();
        int n_partial_factors = position.at(g_num).cols();
        one_param_vet.setZero(1,n_partial_factors);
        avt_input_class m_treats1fact;
        m_xtreats_xfacts.clear();

        for  (int i=0;i<n_partial_factors;i++)
        {
           for  (int j=0;j<n_treat;j++)
           {
                one_param_vet(0,i)=(double)(j+1);
                m_treats1fact.fct_cmbntn.append(one_param_vet);
                one_param_vet.setZero(1,n_partial_factors);
            }
            m_xtreats_xfacts.append(m_treats1fact);
            m_treats1fact.fct_cmbntn.clear();
        }

        //Make the combination of treatment for all factors (Ntreat^Nfact) vectors
        avt_input_class f_vet_tbc;
        f_vet_tbc = m_xtreats_xfacts.at(0);

        avt_input_class g_comb_tmp = make_combination(n_treat,n_partial_factors,f_vet_tbc,m_xtreats_xfacts,false,true);

        MatrixXd g_comb;
        int num_comb_of_treat;
        num_comb_of_treat=g_comb_tmp.fct_cmbntn.count();

        int takeit, ll, m;

        takeit=0;
        ll=0;
        m=1;

        for (int s=0;s<n_comb_sempl;s++)  // cycle on all the ordered selections without repetition of g_num interaction order
        {
            for (int h=0;h<num_comb_of_treat;h++) // cycle on the number of the combined factorsi
            {
                for (int d=0;d<n_combination;d++) // cycle on all the combinations for all the factors
                {
                    for (int j=0;j<n_partial_factors;j++)   // find the combinations that have the desired value for the selected factors
                    {
                        if (m_matrix_propagated.fct_cmbntn.at(d)(0,position.at(g_num)(s,j))== g_comb_tmp.fct_cmbntn.at(h)(0,j))
                            takeit = takeit + 1;
                    }
                    if (takeit == n_partial_factors)
                    {
                        h_tmp.takecombination.append(m_matrix_propagated.fct_cmbntn.at(d));
                        h_tmp.takesamples.append(m_matrix_propagated.samples.at(d));
                    }
                    takeit=0;
                }
                ok_tmp.h.append(h_tmp);
                h_tmp.takecombination.clear();
                h_tmp.takesamples.clear();
            }
            // append in s
            ok.append(ok_tmp);
            ok_tmp.h.clear();
       }

//     Calculate Sums of Squares ---------------------------------------------

        SS_struct SS_comb_sempl_tmp;
        SS_comb_sempl_tmp.SSwithinn.clear();

        for(int s=0;s<n_comb_sempl;s++)
        {
            MatrixXd SSwithin_tmp;
            SS_struct SS_tmp;
            VectorXd simple_comb_tmp;

            SSwithin_tmp.setZero(num_comb_of_treat,n_col);

            for (int h=0;h<num_comb_of_treat;h++)
            {
                MatrixXd calderone;
                int n_sub_sample=ok.at(s).h.at(h).takesamples.count();
                calderone.setZero(n_sub_sample*n_samples,n_col);

                for (int ll=0;ll<n_sub_sample;ll++)
                {
                    calderone.block((ll)*n_samples,0,n_samples,n_col) = ok.at(s).h.at(h).takesamples.at(ll);
                }
                VectorXd ss_calderone = calculate_ss(calderone);
                SSwithin_tmp.block(h,0,1,6) = ss_calderone.transpose();
            }

              VectorXd SS_tmp_y;
              SS_tmp_y.setZero(6);

              for (int y=0;y<SSwithin_tmp.cols();y++)
              {
                  SS_tmp_y(y) = SSwithin_tmp.col(y).sum();
              }
              SS_comb_sempl_tmp.SSwithinn.append(SS_tmp_y);
              SS_comb_sempl_tmp.simple_combinations.append(position.at(g_num).row(s));
              SS_comb_sempl_tmp.dof=pow((this->n_treat-1),(double)(g_num+1));
        }
        m_SS.append(SS_comb_sempl_tmp);
        ok.clear();
    }

    for (int g_num=0;g_num<max_pos;g_num++)
    {
        int n_comb_sempl= position.at(g_num).rows();
        SS_struct SS_current_gnum;
        SS_current_gnum=m_SS.at(g_num);

        for (int s=0;s<n_comb_sempl;s++)    // cycle on all the ordered selections without repetition of g_num interaction order
        {
            SS_current_gnum.SSbetween.append(SStot-SS_current_gnum.SSwithinn.at(s));
            if (g_num>0)
            {
                int current_sample_dim = m_SS.at(g_num).simple_combinations.at(s).rows();
                //Calulate SSbetween (interaction)
                for (int i=0;i<g_num;i++) //  cycle on the SS with an interaction order lower than g_num
                {
                    int n_comb_sampl = m_SS.at(i).simple_combinations.count();
                    for (int ss=0;ss<n_comb_sampl;ss++) // cycle on all the ordered selections without repetition of i interaction order
                    {
                        int analysed_sample_dim = m_SS.at(i).simple_combinations.at(ss).rows();
                        int control=0;
                        for (int z=0;z<analysed_sample_dim;z++) // cycle on the number of factors of the analyzed combination
                        {
                            for (int y=0;y<current_sample_dim;y++) // cycle on the number of factors of the current combination
                            {                              
                                int a = m_SS.at(i).simple_combinations.at(ss)(z);
                                int b = m_SS.at(g_num).simple_combinations.at(s)(y);
                                //Comparison between factors
                                if (a==b)
                                {
                                    control=control+1;
                                }
                            }
                        }
                        if (control==analysed_sample_dim)
                        {
                            VectorXd new_ssb;
                            new_ssb.setZero(n_col);
                            new_ssb = SS_current_gnum.SSbetween.at(s)-m_SS.at(i).SSbetween.at(ss);
                            SS_current_gnum.SSbetween.replace(s,new_ssb);
                        }
                    }

                }
            }
            //MS calculation
            VectorXd one_MS=SS_current_gnum.SSbetween.at(s)/SS_current_gnum.dof;
            SS_current_gnum.MS.append(one_MS);
        }
        m_SS.replace(g_num,SS_current_gnum);
    }

    // Error calculation
    m_error=SStot;
    for (int g_num=0;g_num<max_pos;g_num++)
    {
        int n_comb_sempl= position.at(g_num).rows();
        for (int s=0;s<n_comb_sempl;s++)
        {
            m_error=m_error-m_SS.at(g_num).SSbetween.at(s);
        }
    }
    m_dof_err=(n_samples-1)*pow(n_treat,(double)n_factors);
    m_errorMS=m_error/m_dof_err;

    // F-factor calculation
    for (int g_num=0;g_num<max_pos;g_num++)
    {
        SS_struct SS_current_gnum;
        SS_current_gnum=m_SS.at(g_num);
        int n_comb_sempl= position.at(g_num).rows();
        for (int s=0;s<n_comb_sempl;s++)
        {
            VectorXd one_F;
            one_F.setZero(n_col);
            for (int k=0; k<n_col;k++)
                one_F(k)=SS_current_gnum.MS.at(s)(k) /(m_errorMS)(k);
            SS_current_gnum.F_factor.append(one_F);
        }
        m_SS.replace(g_num,SS_current_gnum);
    }

//    PRINT RESULTS-----------------------------------------------------
    QString nome_file = m_directory+"ANOVA_time_step_"+variation_ts+".txt";
    const char* nome_file_char=(const char*) nome_file.toLatin1();
    FILE* wFile = fopen (nome_file_char,"w");

    fprintf(wFile,"SStot \n");
    print_vector(SStot,wFile);
    for (int g_num=0;g_num<max_pos;g_num++)
    {
        int n_comb_sempl= position.at(g_num).rows();
        fprintf(wFile,"###########Interaction order: %d ################\n",g_num+1);

        for (int s=0;s<n_comb_sempl;s++)  //
        {
            fprintf(wFile,"-------------Interaction %d ----------------\n",s+1);
            fprintf(wFile,"DOF \n");
            fprintf(wFile,"%d \n",m_SS.at(g_num).dof);
            fprintf(wFile,"SSwithin \n");
            print_vector(m_SS.at(g_num).SSwithinn.at(s),wFile);
            fprintf(wFile,"SSbetween \n");
            print_vector(m_SS.at(g_num).SSbetween.at(s),wFile);
            fprintf(wFile,"MS \n");
            print_vector(m_SS.at(g_num).MS.at(s),wFile);
            fprintf(wFile,"F-factor \n");
            print_vector(m_SS.at(g_num).F_factor.at(s),wFile);
            fprintf(wFile,"\n");
        }
        fprintf(wFile,"\n");
    }
    fprintf(wFile,"Error \n");
    fprintf(wFile,"Error DOF: %d\n",m_dof_err);
    print_vector(m_error,wFile);
    fprintf(wFile,"MS errore \n");
    print_vector(m_errorMS,wFile);
    fclose(wFile);

    //    PRINT SAMPLE FOR VERIFICATION-----------------------------------------------------
    nome_file = m_directory+"Sample_time_step_"+variation_ts+".txt";
    nome_file_char=(const char*) nome_file.toLatin1();
    wFile = fopen (nome_file_char,"w");

    for (int i=0;i<n_combination;i++)
    {
        for (int j=0;j<n_samples;j++)
        {
            for(int k=0;k<n_col;k++)
                fprintf(wFile,"%f\t",m_matrix_propagated.samples.at(i)(j,k));
            for(int g=0;g<n_factors;g++)
                fprintf(wFile,"%f\t",m_matrix_propagated.fct_cmbntn.at(i)(0,g));
            fprintf(wFile,"\n");
        }
    }
    fclose(wFile);
}

MatrixXi AvtAnalysis::cycle(VectorXi fact_list, MatrixXi g)
{
 int z=0;
 int k=g.cols()+1;
 MatrixXi t;

 int dim=(factorial(n_factors))/(factorial(k)*factorial(n_factors-k));
 t.setZero(dim,k);

 for (int i=0;i<g.rows();i++)
 {
     for (int h=g(i,k-2)+1;h<n_factors;h++)
     {
         t.block(z,0,1,g.cols())=g.row(i);
         if (g(i,k-2)+1<n_factors)
         {
              t(z,k-1)=h;
              z=z+1;
              if (z>=dim)return t;
         }
      }
  }
  return t;
}

void AvtAnalysis::print_matrix(MatrixXd matrix,FILE* wFile)
{
    for (int g=0 ; g<matrix.rows(); g++)
    {
        for (int z=0 ; z<matrix.cols(); z++)
        {
            fprintf (wFile, "%f \t",matrix(g,z));
        }
        fprintf (wFile,"\n");
    }
    fprintf (wFile,"\n");
}


void AvtAnalysis::print_matrix_int(MatrixXi matrix,FILE* wFile)
{
    for (int g=0 ; g<matrix.rows(); g++)
    {
        for (int z=0 ; z<matrix.cols(); z++)
        {
            fprintf (wFile, "%d \t",matrix(g,z));
        }
        fprintf (wFile,"\n");

    }
    fprintf (wFile,"\n");
}

void AvtAnalysis::print_vector(VectorXd vector,FILE* wFile)
{
    for (int g=0 ; g<vector.size(); g++)
    {
         fprintf (wFile, "%6f \t",vector(g));
    }
    fprintf (wFile,"\n");
}

void AvtAnalysis::print_vector_int(VectorXi vector,FILE* wFile)
{
    for (int g=0 ; g<vector.size(); g++)
    {
         fprintf (wFile, "%d \t",vector(g));
    }
    fprintf (wFile,"\n");
}

int AvtAnalysis::save_propagation(QList<MatrixXd> cov, QList<MatrixXd> samples)
{
    m_matrix_propagated.treatment_mat=cov;
    m_matrix_propagated.samples=samples;
    m_matrix_propagated.fct_cmbntn = m_matrix_combined.fct_cmbntn;
}

VectorXd AvtAnalysis::calculate_ss(MatrixXd mat)
{
    VectorXd ss,tmp;
    double sum, mean;
    int n_ss = mat.cols();
    int n_elem = mat.rows();

    ss.setZero(n_ss);

    for (int i=0;i<n_ss;i++)
    {
        tmp.setZero(n_elem);
        mean = mat.col(i).sum()/mat.col(i).count();
        for (int j=0;j<n_elem;j++)
        {
            tmp(j) =pow(mat(j,i) - mean,2);
        }
        ss(i)=tmp.sum();
    }
    return ss;
}

int AvtAnalysis::factorial(int number)
{
    int res = 1;
    for(int i=0;i<number;i++)
        res=res * (i+1);
    return res;
}

AvtAnalysis::~AvtAnalysis()
{

}

