/***************************************************************************
                                   ofx2qif.c
                             -------------------
    copyright            : (C) 2002 by Benoit Gr�goire
    email                : bock@step.polymtl.ca
***************************************************************************/
/**@file
 * \brief Code for ofx2qif utility.  C example code
 *
 * ofx2qif is a OFX "file" to QIF (Quicken Interchange Format)
 converter.  It was written as a second code example, and as a way for LibOFX
 to immediately provide something usefull, and to give people a reason to try
 the library.  It is not recommended that financial software use the
 output of this utility for OFX support.  The QIF file format is very
 primitive, and much information is lost.  The utility curently supports
 every tansaction tags of the QIF format except the address lines, and supports
 many of the !Account tags.  It should generate QIF files that will import 
 sucesfully in just about every software with QIF support.
 *
 * I do not plan on improving working this utility much further, however be I 
 would be more than happy to accept contributions.  If you are interested in 
 hacking on ofx2qif, links to QIF documentation are available on the LibOFX
 home page.
 *
 * ofx2qif is meant to be the C code example and demo of the library.  It uses
 many of the functions and structures of the LibOFX API.  Note that unlike 
 ofxdump, all error output is disabled by default.
 *
 * usage: ofx2qif path_to_ofx_file/ofx_filename > output_filename.qif
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "libofx.h"

#define QIF_FILE_MAX_SIZE 256000 
static char trans_list_buff[QIF_FILE_MAX_SIZE];
int trans_list_is_empty;

int main (int argc, char *argv[])
{
extern int ofx_PARSER_msg;
extern int ofx_DEBUG_msg;
extern int ofx_WARNING_msg;
extern int ofx_ERROR_msg;
extern int ofx_INFO_msg;
extern int ofx_STATUS_msg;
 ofx_PARSER_msg = false;
 ofx_DEBUG_msg = false;
 ofx_WARNING_msg = false;
 ofx_ERROR_msg = false;
 ofx_INFO_msg = false;
 ofx_STATUS_msg = false;

ofx_proc_file(argc, argv);
return 0;
}

int ofx_proc_status_cb(struct OfxStatusData data)
{
return 0;
}
int ofx_proc_security_cb(struct OfxSecurityData data)
{
return 0;
}
int ofx_proc_transaction_cb(struct OfxTransactionData data)
{
  char dest_string[255];
  char trans_buff[4096];
  struct tm temp_tm;

  trans_list_is_empty=false;

  if(data.date_posted_valid==true){
    temp_tm = *localtime(&(data.date_posted));
    sprintf(trans_buff, "D%d%s%d%s%d%s", temp_tm.tm_mday, "/", temp_tm.tm_mon+1, "/", temp_tm.tm_year+1900, "\n");
    strncat(trans_list_buff, trans_buff, QIF_FILE_MAX_SIZE - strlen(trans_list_buff));
  }
  if(data.amount_valid==true){
    sprintf(trans_buff, "T%.2f%s",data.amount,"\n");
    strncat(trans_list_buff, trans_buff, QIF_FILE_MAX_SIZE - strlen(trans_list_buff));
  }
  if(data.check_number_valid==true){
    sprintf(trans_buff, "N%s%s",data.check_number,"\n");
    strncat(trans_list_buff, trans_buff, QIF_FILE_MAX_SIZE - strlen(trans_list_buff));
  }
  else if(data.reference_number_valid==true){
    sprintf(trans_buff, "N%s%s",data.reference_number,"\n");
      strncat(trans_list_buff, trans_buff, QIF_FILE_MAX_SIZE - strlen(trans_list_buff));
}
if(data.name_valid==true){
    sprintf(trans_buff, "P%s%s",data.name,"\n");
        strncat(trans_list_buff, trans_buff, QIF_FILE_MAX_SIZE - strlen(trans_list_buff));
}
if(data.memo_valid==true){
    sprintf(trans_buff, "M%s%s",data.memo,"\n");
        strncat(trans_list_buff, trans_buff, QIF_FILE_MAX_SIZE - strlen(trans_list_buff));
}
/* Add PAYEE and ADRESS here once supported by the library */


if(data.transactiontype_valid==true){
    switch(data.transactiontype){
        case OFX_CREDIT: strncpy(dest_string, "Generic credit", sizeof(dest_string));
        break;
        case OFX_DEBIT: strncpy(dest_string, "Generic debit", sizeof(dest_string));
        break;
        case OFX_INT: strncpy(dest_string, "Interest earned or paid (Note: Depends on signage of amount)", sizeof(dest_string));
        break;
        case OFX_DIV: strncpy(dest_string, "Dividend", sizeof(dest_string));
        break;
        case OFX_FEE: strncpy(dest_string, "FI fee", sizeof(dest_string));
        break;
        case OFX_SRVCHG: strncpy(dest_string, "Service charge", sizeof(dest_string));
        break;
        case OFX_DEP: strncpy(dest_string, "Deposit", sizeof(dest_string));
        break;
        case OFX_ATM: strncpy(dest_string, "ATM debit or credit (Note: Depends on signage of amount)", sizeof(dest_string));
        break;
        case OFX_POS: strncpy(dest_string, "Point of sale debit or credit (Note: Depends on signage of amount)", sizeof(dest_string));
        break;
        case OFX_XFER: strncpy(dest_string, "Transfer", sizeof(dest_string));
        break;
        case OFX_CHECK: strncpy(dest_string, "Check", sizeof(dest_string));
        break;
        case OFX_PAYMENT: strncpy(dest_string, "Electronic payment", sizeof(dest_string));
        break;
        case OFX_CASH: strncpy(dest_string, "Cash withdrawal", sizeof(dest_string));
        break;
        case OFX_DIRECTDEP: strncpy(dest_string, "Direct deposit", sizeof(dest_string));
        break;
        case OFX_DIRECTDEBIT: strncpy(dest_string, "Merchant initiated debit", sizeof(dest_string));
        break;
        case OFX_REPEATPMT: strncpy(dest_string, "Repeating payment/standing order", sizeof(dest_string));
        break;
        case OFX_OTHER: strncpy(dest_string, "Other", sizeof(dest_string));
        break;
        default : strncpy(dest_string, "Unknown transaction type", sizeof(dest_string));
        break;
    }
    sprintf(trans_buff, "L%s%s",dest_string,"\n");
    strncat(trans_list_buff, trans_buff, QIF_FILE_MAX_SIZE - strlen(trans_list_buff));
}
 sprintf(trans_buff, "^\n");
 strncat(trans_list_buff, trans_buff, QIF_FILE_MAX_SIZE - strlen(trans_list_buff));
 return 0;
}/* end ofx_proc_transaction() */

int ofx_proc_statement_cb(struct OfxStatementData data)
{
  struct tm temp_tm;

  printf("!Account\n");
  if(data.account_id_valid==true){
    /* Use the account id as the qif name of the account */
    printf("N%s%s",data.account_id,"\n");
  }
  if(data.account_ptr->account_type_valid==true)
    {
      switch(data.account_ptr->account_type){
      case OFX_CHECKING : printf("TBank\n");
	break;
      case OFX_SAVINGS :  printf("TBank\n");
	break;
      case OFX_MONEYMRKT :  printf("TOth A\n");
	break;
      case OFX_CREDITLINE :  printf("TOth L\n");
	break;
      case OFX_CMA :  printf("TOth A\n");
	break;
      case OFX_CREDITCARD :   printf("TCCard\n");
	break;
      default: perror("WRITEME: ofx_proc_account() This is an unknown account type!");
      }
    }
  printf("DOFX online account\n");

  if(data.ledger_balance_date_valid==true){
    temp_tm = *localtime(&(data.ledger_balance_date));
    printf("/%d%s%d%s%d%s", temp_tm.tm_mday, "/", temp_tm.tm_mon+1, "/", temp_tm.tm_year+1900, "\n");
  }
  if(data.ledger_balance_valid==true){
    printf("$%.2f%s",data.ledger_balance,"\n");
  }
  printf("^\n");
  if(trans_list_is_empty==false)
    {
      printf(trans_list_buff);
    }
  trans_list_buff[0]=0;
  return 0;
}/* end ofx_proc_statement() */

int ofx_proc_account_cb(struct OfxAccountData data)
{
  char dest_string[255];

  trans_list_is_empty = true;
  trans_list_buff[0]=0;
  
  if(data.account_type_valid==true){
    switch(data.account_type){
    case OFX_CHECKING : strncpy(dest_string,"!Type:Bank\n",sizeof(dest_string));
      break;
    case OFX_SAVINGS : strncpy(dest_string,"!Type:Bank\n",sizeof(dest_string));
      break;
    case OFX_MONEYMRKT : strncpy(dest_string,"!Type:Oth A\n",sizeof(dest_string));
      break;
    case OFX_CREDITLINE : strncpy(dest_string,"!Type:Oth L\n",sizeof(dest_string));
      break;
    case OFX_CMA : strncpy(dest_string,"!Type:Oth A\n",sizeof(dest_string));
      break;
    case OFX_CREDITCARD : strncpy(dest_string,"!Type:CCard\n",sizeof(dest_string));
      break;
    default: perror("WRITEME: ofx_proc_account() This is an unknown account type!");
    }
    strncat(trans_list_buff, dest_string, QIF_FILE_MAX_SIZE - strlen(trans_list_buff));
  }
  return 0;
}/* end ofx_proc_account() */
