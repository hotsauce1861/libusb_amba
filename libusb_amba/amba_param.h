#ifndef AMBA_PARAM_H
#define AMBA_PARAM_H


class amba_param
{
public:
    amba_param();

    int             get_vid(){return m_vid;}
    int             get_pid(){return m_pid;}
    char            get_endpoint_in(){return m_endpoint_in;}
    char            get_endpoint_out(){return m_endpoint_out;}

    void            set_vid(char _vid){m_vid = _vid;}
    void            set_pid(char _pid){m_pid = _pid;}

    void            set_endpoint_in(int _edp_in){m_endpoint_in = _edp_in;}
    void            set_endpoint_out(int _edp_out){m_endpoint_out = _edp_out;}

private:

    int             m_vid;
    int             m_pid;
    char            m_endpoint_in;
    char            m_endpoint_out;

};

#endif // AMBA_PARAM_H
