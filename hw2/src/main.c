
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include "version.h"
#include "global.h"
#include "gradedb.h"
#include "stats.h"
#include "read.h"
#include "write.h"
#include "normal.h"
#include "sort.h"
#include "report.h"
/*
 * Course grade computation program
 */

#define REPORT          0
#define COLLATE         1
#define FREQUENCIES     2
#define QUANTILES       3
#define SUMMARIES       4
#define MOMENTS         5
#define COMPOSITES      6
#define INDIVIDUALS     7
#define HISTOGRAMS      8
#define TABSEP          9
#define ALLOUTPUT      10
#define SORTBY         12
#define NONAMES        11
#define OUTPUT         13

static struct option_info {
        unsigned int val;
        char *name;
        char chr;
        int has_arg;
        char *argname;
        char *descr;
} option_table[] = {
 {REPORT,         "report",    'r',      no_argument, NULL,
                  "Process input data and produce specified reports."},
 {COLLATE,        "collate",   'c',      no_argument, NULL,
                  "Collate input data and dump to standard output."},
 {FREQUENCIES,    "freqs",     0,        no_argument, NULL,
                  "Print frequency tables."},
 {QUANTILES,      "quants",    0,        no_argument, NULL,
                  "Print quantile information."},
 {SUMMARIES,      "summaries", 0,        no_argument, NULL,
                  "Print quantile summaries."},
 {MOMENTS,        "stats",     0,        no_argument, NULL,
                  "Print means and standard deviations."},
 {COMPOSITES,     "comps",     0,        no_argument, NULL,
                  "Print students' composite scores."},
 {INDIVIDUALS,    "indivs",    0,        no_argument, NULL,
                  "Print students' individual scores."},
 {HISTOGRAMS,     "histos",    0,        no_argument, NULL,
                  "Print histograms of assignment scores."},
 {TABSEP,         "tabsep",    0,        no_argument, NULL,
                  "Print tab-separated table of student scores."},
 {ALLOUTPUT,      "all",       'a',      no_argument, NULL,
                  "Print all reports."},
 {NONAMES,        "nonames",   'n',      no_argument, NULL,
                  "Suppress printing of students' names."},
 {SORTBY,         "sortby",    'k',      required_argument, "key",
                  "Sort by {name, id, score}."},
 {OUTPUT,         "output",     'o',     required_argument,"outfile",
                  "Write output to file,rather than standard output."},
 {0}
};

#define NUM_OPTIONS (15)

static char *short_options = "rcak:no:";
static struct option long_options[NUM_OPTIONS];

static void init_options() {
    for(unsigned int i = 0; i < NUM_OPTIONS; i++) {
        struct option_info *oip = &option_table[i];
        struct option *op = &long_options[i];
        op->name = oip->name;
        op->has_arg = oip->has_arg;
        op->flag = NULL;
        op->val = oip->val;
    }
}

static int report, collate, freqs, quantiles, summaries, moments,
           scores, composite, histograms, tabsep, nonames,output;

static void usage();
int errors;
int main(argc, argv)
int argc;
char *argv[];
{
    Course *c;
    Stats *s;
    extern int errors, warnings;
    char optval;
    char* outfile;
    int (*compare)() = comparename;
    int option_index = 1;
    fprintf(stderr, BANNER);
    init_options();
    if(argc <= 1) usage(argv[0]);
    while(optind < argc) {
        if((optval = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
            switch(optval) {
            case REPORT:
            case 'r':
                if(option_index==1){
                    report++;
                    option_index++;
                }
                else{
                    fprintf(stderr,"Report option not at correct position\n");
                    usage(argv[0]);
                }
                break;
            case COLLATE:
            case 'c':
                if(option_index==1){
                    collate++;
                    option_index++;
                }
                else{
                    fprintf(stderr,"Collate option not at correct position\n");
                    usage(argv[0]);
                }
                break;
            case TABSEP: tabsep++; option_index++;break;
            case NONAMES:
            case 'n': nonames++; option_index++; break;
            case SORTBY:
            case 'k':
                if(!strcmp(optarg, "name"))
                    compare = comparename;
                else if(!strcmp(optarg, "id"))
                    compare = compareid;
                else if(!strcmp(optarg, "score"))
                    compare = comparescore;
                else {
                    debug("optval is:%c",optval);
                    fprintf(stderr,
                            "Option '%s' requires argument from {name, id, score}.\n\n",
                            option_table[SORTBY].name);
                    usage(argv[0]);
                }
                option_index+=2;
                break;
            case OUTPUT:
            case 'o': output++;
                outfile=optarg; option_index+=2;  break;
            case FREQUENCIES: freqs++; option_index++;break;
            case QUANTILES: debug("quantiles");quantiles++; option_index++;debug("option_idex quantile:%d",option_index);break;
            case SUMMARIES: summaries++; option_index++;break;
            case MOMENTS: moments++; option_index++;break;
            case COMPOSITES: composite++; option_index++;break;
            case INDIVIDUALS: scores++; option_index++;break;
            case HISTOGRAMS: histograms++; option_index++;break;
            case ALLOUTPUT:
            case 'a':
                freqs++; quantiles++; summaries++; moments++;
                composite++; scores++; histograms++; tabsep++;option_index++;
                break;
            case '?':
                usage(argv[0]);
                break;
            default:
                break;
            }
        } else {
            break;
        }
    }
    if(optind == argc) {
            fprintf(stderr, "No input file specified.\n\n");
            usage(argv[0]);
    }
    char *ifile = argv[optind];

    if(report == collate) {
            fprintf(stderr, "Exactly one of '%s' or '%s' is required.\n\n",
                    option_table[REPORT].name, option_table[COLLATE].name);
            usage(argv[0]);
    }
    if(report > 1 || collate > 1 || freqs >1 || quantiles > 1 || summaries >1 || moments > 1 || composite > 1 || scores > 1 || histograms > 1 || tabsep > 1){
        usage(argv[0]);
    }
    fprintf(stderr, "Reading input data...\n");
    c = readfile(ifile);
    if(errors) {
       printf("%d error%s found, so no computations were performed.\n",
              errors, errors == 1 ? " was": "s were");
       exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Calculating statistics...\n");
    s = statistics(c);
    if(s == NULL) fatal("There is no data from which to generate reports.");
    normalize(c);
    composites(c);
    sortrosters(c, comparename);
    checkfordups(c->roster);
    FILE* out_fd;
    if(output){
        if((out_fd= fopen(outfile, "w")) == NULL)
            fatal("Can't open data file %s.\n", outfile);
    }
    else
        out_fd=stdout;
    if(collate) {
            fprintf(stderr, "Dumping collated data...\n");
            writecourse(out_fd, c);
            exit(errors ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    sortrosters(c, compare);

    fprintf(stderr, "Producing reports...\n");

    reportparams(out_fd, ifile, c);
    if(moments) reportmoments(out_fd, s);
    if(composite) reportcomposites(out_fd, c, nonames);
    if(freqs) reportfreqs(out_fd, s);
    if(quantiles) reportquantiles(out_fd, s);
    if(summaries) reportquantilesummaries(out_fd, s);
    if(histograms) reporthistos(out_fd, c, s);
    if(scores) reportscores(out_fd, c, nonames);
    if(tabsep) reporttabs(out_fd, c);
    //if(tabsep) reporttabs(stdout, c);
    fclose(out_fd);
    fprintf(stderr, "\nProcessing complete.\n");
    printf("%d warning%s issued.\n", warnings+errors,
           warnings+errors == 1? " was": "s were");

    exit(errors ? EXIT_FAILURE : EXIT_SUCCESS);
}

void usage(name)
char *name;
{
        struct option_info *opt;

        fprintf(stderr, "Usage: %s [options] <data file>\n", name);
        fprintf(stderr, "Valid options are:\n");
        for(unsigned int i = 0; i < NUM_OPTIONS; i++) {

                opt = &option_table[i];
                char optchr[5] = {' ', ' ', ' ', ' ', '\0'};
                if(opt->chr)
                  sprintf(optchr, "-%c, ", opt->chr);
                char arg[32];
                if(opt->has_arg)
                    sprintf(arg, " <%.10s>", opt->argname);
                else
                    sprintf(arg, "%.13s", "");
                fprintf(stderr, "\t%s--%-10s%-13s\t%s\n",
                            optchr, opt->name, arg, opt->descr);
                opt++;
        }
        exit(EXIT_FAILURE);
}

