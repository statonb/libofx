/***************************************************************************
                                   ofx2qif.c
                             -------------------
    copyright            : (C) 2002 by Benoit Gr�goire
    email                : benoitg@coeus.ca
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
#include <getopt.h>
#include "libofx.h"

#define QIF_FILE_MAX_SIZE 256000

FILE *fpOut;

const char *progName = "ofx2qif";
const char *verString = "0.9.?";

void usage(const char *exeName)
{
    fprintf(stderr, "usage: %s -i inFile [-o outFile]\n", exeName);
}

int ofx_proc_transaction_cb(const struct OfxTransactionData data, void * transaction_data)
{
    char dest_string[255];
    char trans_buff[4096];
    struct tm temp_tm;
    char trans_list_buff[QIF_FILE_MAX_SIZE];

    trans_list_buff[0]='\0';

    if(data.date_posted_valid==true)
    {
        temp_tm = *localtime(&(data.date_posted));
        sprintf(trans_buff, "D%d%s%d%s%d%s", temp_tm.tm_mon+1, "/", temp_tm.tm_mday, "/", temp_tm.tm_year+1900, "\n");
        strncat(trans_list_buff, trans_buff, sizeof(trans_list_buff)-1 - strlen(trans_list_buff));
    }
    if(data.amount_valid==true)
    {
        sprintf(trans_buff, "T%.2f%s",data.amount,"\n");
        strncat(trans_list_buff, trans_buff, sizeof(trans_list_buff)-1 - strlen(trans_list_buff));
    }
    if(data.check_number_valid==true)
    {
        sprintf(trans_buff, "N%s%s",data.check_number,"\n");
        strncat(trans_list_buff, trans_buff, sizeof(trans_list_buff)-1 - strlen(trans_list_buff));
    }
    else if(data.reference_number_valid==true)
    {
        sprintf(trans_buff, "N%s%s",data.reference_number,"\n");
        strncat(trans_list_buff, trans_buff, sizeof(trans_list_buff)-1 - strlen(trans_list_buff));
    }
    if(data.name_valid==true)
    {
        sprintf(trans_buff, "P%s%s",data.name,"\n");
        strncat(trans_list_buff, trans_buff, sizeof(trans_list_buff)-1 - strlen(trans_list_buff));
    }
    if(data.memo_valid==true)
    {
        sprintf(trans_buff, "M%s%s",data.memo,"\n");
        strncat(trans_list_buff, trans_buff, sizeof(trans_list_buff)-1 - strlen(trans_list_buff));
    }
    /* Add PAYEE and ADRESS here once supported by the library */


    if(data.transactiontype_valid==true)
    {
        switch(data.transactiontype)
        {
        case OFX_CREDIT:
            strncpy(dest_string, "Generic credit", sizeof(dest_string));
            break;
        case OFX_DEBIT:
            strncpy(dest_string, "Generic debit", sizeof(dest_string));
            break;
        case OFX_INT:
            strncpy(dest_string, "Interest earned or paid (Note: Depends on signage of amount)", sizeof(dest_string));
            break;
        case OFX_DIV:
            strncpy(dest_string, "Dividend", sizeof(dest_string));
            break;
        case OFX_FEE:
            strncpy(dest_string, "FI fee", sizeof(dest_string));
            break;
        case OFX_SRVCHG:
            strncpy(dest_string, "Service charge", sizeof(dest_string));
            break;
        case OFX_DEP:
            strncpy(dest_string, "Deposit", sizeof(dest_string));
            break;
        case OFX_ATM:
            strncpy(dest_string, "ATM debit or credit (Note: Depends on signage of amount)", sizeof(dest_string));
            break;
        case OFX_POS:
            strncpy(dest_string, "Point of sale debit or credit (Note: Depends on signage of amount)", sizeof(dest_string));
            break;
        case OFX_XFER:
            strncpy(dest_string, "Transfer", sizeof(dest_string));
            break;
        case OFX_CHECK:
            strncpy(dest_string, "Check", sizeof(dest_string));
            break;
        case OFX_PAYMENT:
            strncpy(dest_string, "Electronic payment", sizeof(dest_string));
            break;
        case OFX_CASH:
            strncpy(dest_string, "Cash withdrawal", sizeof(dest_string));
            break;
        case OFX_DIRECTDEP:
            strncpy(dest_string, "Direct deposit", sizeof(dest_string));
            break;
        case OFX_DIRECTDEBIT:
            strncpy(dest_string, "Merchant initiated debit", sizeof(dest_string));
            break;
        case OFX_REPEATPMT:
            strncpy(dest_string, "Repeating payment/standing order", sizeof(dest_string));
            break;
        case OFX_OTHER:
            strncpy(dest_string, "Other", sizeof(dest_string));
            break;
        default :
            strncpy(dest_string, "Unknown transaction type", sizeof(dest_string));
            break;
        }
        sprintf(trans_buff, "L%s%s",dest_string,"\n");
        strncat(trans_list_buff, trans_buff, sizeof(trans_list_buff)-1 - strlen(trans_list_buff));
    }
    strcpy(trans_buff, "^\n");
    strncat(trans_list_buff, trans_buff, sizeof(trans_list_buff)-1 - strlen(trans_list_buff));
    fputs(trans_list_buff,fpOut);
    return 0;
}/* end ofx_proc_transaction() */

int ofx_proc_statement_cb(const struct OfxStatementData data, void * statement_data)
{
    struct tm temp_tm;

    fprintf(fpOut, "!Account\n");
    if(data.account_id_valid==true)
    {
        /* Use the account id as the qif name of the account */
        fprintf(fpOut, "N%s%s",data.account_id,"\n");
    }
    if(data.account_ptr->account_type_valid==true)
    {
        switch(data.account_ptr->account_type)
        {
        case OFX_CHECKING :
            fprintf(fpOut, "TBank\n");
            break;
        case OFX_SAVINGS :
            fprintf(fpOut, "TBank\n");
            break;
        case OFX_MONEYMRKT :
            fprintf(fpOut, "TOth A\n");
            break;
        case OFX_CREDITLINE :
            fprintf(fpOut, "TOth L\n");
            break;
        case OFX_CMA :
            fprintf(fpOut, "TOth A\n");
            break;
        case OFX_CREDITCARD :
            fprintf(fpOut, "TCCard\n");
            break;
        default:
            perror("WRITEME: ofx_proc_account() This is an unknown account type!");
        }
    }
    fprintf(fpOut, "DOFX online account\n");

    if(data.ledger_balance_date_valid==true)
    {
        temp_tm = *localtime(&(data.ledger_balance_date));
        fprintf(fpOut, "/%d%s%d%s%d%s", temp_tm.tm_mon+1, "/", temp_tm.tm_mday, "/", temp_tm.tm_year+1900, "\n");
    }
    if(data.ledger_balance_valid==true)
    {
        fprintf(fpOut, "$%.2f%s",data.ledger_balance,"\n");
    }
    fprintf(fpOut, "^\n");
    /*The transactions will follow, here is the header */
    if(data.account_ptr->account_type_valid==true)
    {
        switch(data.account_ptr->account_type)
        {
        case OFX_CHECKING :
            fprintf(fpOut, "!Type:Bank\n");
            break;
        case OFX_SAVINGS :
            fprintf(fpOut, "!Type:Bank\n");
            break;
        case OFX_MONEYMRKT :
           fprintf(fpOut, "!Type:Oth A\n");
            break;
        case OFX_CREDITLINE :
           fprintf(fpOut, "!Type:Oth L\n");
            break;
        case OFX_CMA :
            fprintf(fpOut, "!Type:Oth A\n");
            break;
        case OFX_CREDITCARD :
            fprintf(fpOut, "!Type:CCard\n");
            break;
        default:
            perror("WRITEME: ofx_proc_account() This is an unknown account type!");
        }
    }

    return 0;
}/* end ofx_proc_statement() */

int ofx_proc_account_cb(const struct OfxAccountData data, void * account_data)
{
    char dest_string[255]="";

    //    strncat(trans_list_buff, dest_string, QIF_FILE_MAX_SIZE - strlen(trans_list_buff));
    fputs(dest_string,fpOut);
    return 0;
}/* end ofx_proc_account() */

int main (int argc, char *argv[])
{
    ofx_PARSER_msg = false;
    ofx_DEBUG_msg = false;
    ofx_WARNING_msg = false;
    ofx_ERROR_msg = false;
    ofx_INFO_msg = false;
    ofx_STATUS_msg = false;
    char inputFilename[256];
    char outputFilename[256];

    inputFilename[0] = '\0';
    outputFilename[0] = '\0';

    fpOut = stdout; // Will be overwritten if a valid output filename is provided

    int c;

    while (1)
    {
        static struct option long_options[] =
        {
            {"version", no_argument, 0, 0},
            {"input",      required_argument, 0, 'i'},
            {"output",     required_argument, 0, 'o'},
            {0, 0, 0, 0}
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "i:o:",
                         long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c)
        {
        case 0:
            fprintf(stderr, "version: %s\n", verString);
            return -1;
            break;

        case 'i':
            strcpy(inputFilename, optarg);
            break;
        case 'o':
            strcpy(outputFilename, optarg);
            break;
        case '?':
            /* getopt_long already printed an error message. */
            // fallthrough
        default:
            usage(progName);
            return -1;
            break;

        }
    }

    if ('\0' == inputFilename[0])
    {
        // No --input file provided.
        // If there is a remaining command line argument, then assume
        // that's our input file
        if (optind < argc)
        {
            strcpy(inputFilename, argv[optind]);
        }
        else
        {
            fprintf(stderr, "No input file provided\n");
            usage(progName);
            return -1;
        }
    }

    if ('\0' == outputFilename[0])
    {
        // No --output file provided.
        fpOut = stdout; // Yeah, we initialize fpOut to stdout so this is redundant
    }
    else
    {
        fpOut = fopen(outputFilename, "w");
        if ((FILE *)(NULL) == fpOut)
        {
            fprintf(stderr, "Error opening output file %s\n", outputFilename);
            usage(progName);
            return -1;
        }
    }

    LibofxContextPtr libofx_context = libofx_get_new_context();
    ofx_set_statement_cb(libofx_context, ofx_proc_statement_cb, 0);
    ofx_set_account_cb(libofx_context, ofx_proc_account_cb, 0);
    ofx_set_transaction_cb(libofx_context, ofx_proc_transaction_cb, 0);

    fprintf(stderr, "Parsing input file %s\n", inputFilename);

    libofx_proc_file(libofx_context, inputFilename, OFX);

    fclose(fpOut);

    return libofx_free_context(libofx_context);
}





