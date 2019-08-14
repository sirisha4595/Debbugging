#ifndef REPORT_H
#define REPORT_H
#endif
void histo(FILE *fd, int bins[], float min, float max, int cnt);
void reportparams(FILE *fd, char *fn,Course *c);
void reporthistos(FILE *fd, Course *c, Stats *s);
void reporttabs(FILE *fd, Course *c);
void reportcomposites(FILE *fd, Course *c, int nm);
void reportscores(FILE *fd, Course *c, int nm);
void reportmoments(FILE *fd,  Stats *s);
void reportquantiles(FILE *fd, Stats *s);
void reportquantilesummaries( FILE *fd, Stats *s);
float interpolatequantile(Freqs *fp, int n, float q);
void reportfreqs(FILE *fd, Stats *s);