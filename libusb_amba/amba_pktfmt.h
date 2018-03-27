#ifndef AMBA_PKTFMT_H
#define AMBA_PKTFMT_H

class amba_pktfmt
{
public:
    amba_pktfmt();
    int             get_header_size(){return m_header_size;}

private:
    int             m_header_size;


};

#endif // AMBA_PKTFMT_H
