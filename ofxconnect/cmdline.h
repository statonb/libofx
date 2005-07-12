/* cmdline.h */

/* File autogenerated by gengetopt version 2.11  */

#ifndef CMDLINE_H
#define CMDLINE_H

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef CMDLINE_PARSER_PACKAGE
#define CMDLINE_PARSER_PACKAGE PACKAGE
#endif

#ifndef CMDLINE_PARSER_VERSION
#define CMDLINE_PARSER_VERSION VERSION
#endif

struct gengetopt_args_info
{
  int statement_req_flag;	/* Request for a statement (default=off).  */
  int accountinfo_req_flag;	/* Request for a list of accounts (default=off).  */
  char * fid_arg;	/* FI identifier.  */
  char * org_arg;	/* FI org tag.  */
  char * bank_arg;	/* IBAN bank identifier.  */
  char * broker_arg;	/* Broker identifier.  */
  char * user_arg;	/* User name.  */
  char * pass_arg;	/* Password.  */
  char * acct_arg;	/* Account ID.  */
  int type_arg;	/* Account Type 1=checking 2=invest 3=ccard.  */
  long past_arg;	/* How far back to look from today (in days).  */
  char * url_arg;	/* Url to POST the data to (otherwise goes to stdout).  */

  int help_given ;	/* Whether help was given.  */
  int version_given ;	/* Whether version was given.  */
  int statement_req_given ;	/* Whether statement-req was given.  */
  int accountinfo_req_given ;	/* Whether accountinfo-req was given.  */
  int fid_given ;	/* Whether fid was given.  */
  int org_given ;	/* Whether org was given.  */
  int bank_given ;	/* Whether bank was given.  */
  int broker_given ;	/* Whether broker was given.  */
  int user_given ;	/* Whether user was given.  */
  int pass_given ;	/* Whether pass was given.  */
  int acct_given ;	/* Whether acct was given.  */
  int type_given ;	/* Whether type was given.  */
  int past_given ;	/* Whether past was given.  */
  int url_given ;	/* Whether url was given.  */

  char **inputs ; /* unamed options */
  unsigned inputs_num ; /* unamed options number */
} ;

int cmdline_parser (int argc, char * const *argv, struct gengetopt_args_info *args_info);

void cmdline_parser_print_help(void);
void cmdline_parser_print_version(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* CMDLINE_H */
