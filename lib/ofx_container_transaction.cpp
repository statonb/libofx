/***************************************************************************
         ofx_container_account.cpp 
                             -------------------
    copyright            : (C) 2002 by Benoit Gr�goire
    email                : bock@step.polymtl.ca
***************************************************************************/
/**@file
 * \brief Implementation of  OfxTransactionContainer, 
 OfxBankTransactionContainer and OfxInvestmentTransactionContainer.
*/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include "messages.hh"
#include "libofx.h"
#include "ofx_containers.hh"
#include "ofx_utilities.hh"

extern OfxMainContainer * MainContainer;

/***************************************************************************
 *                      OfxTransactionContainer                            *
 ***************************************************************************/

OfxTransactionContainer::OfxTransactionContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier):
  OfxGenericContainer(para_parentcontainer, para_tag_identifier)
{
  OfxGenericContainer * tmp_parentcontainer=parentcontainer;

  memset(&data,0,sizeof(data));
  type="TRANSACTION";
  /* Find the parent statement container*/
  while(tmp_parentcontainer!=NULL&&tmp_parentcontainer->type!="STATEMENT")
    {
      tmp_parentcontainer=parentcontainer->parentcontainer;
    }  
  if (tmp_parentcontainer!=NULL){
    parent_statement=(OfxStatementContainer*)tmp_parentcontainer;
  }
  else{
    parent_statement=NULL;
    message_out(ERROR,"Unable to find the enclosing statement container this transaction");
  }
  if (parent_statement!=NULL&&parent_statement->data.account_id_valid==true){
    strncpy(data.account_id,parent_statement->data.account_id,OFX_ACCOUNT_ID_LENGTH);
    data.account_id_valid = true;
  }
}
OfxTransactionContainer::~OfxTransactionContainer()
{

}

int OfxTransactionContainer::gen_event()
{
  if(data.unique_id_valid==true&&MainContainer != NULL)
    {
      data.security_data_ptr = MainContainer->find_security(data.unique_id);
      if(data.security_data_ptr!=NULL)
	{
	  data.security_data_valid = true;
	}
    }
  ofx_proc_transaction_cb(data);
  return true;
}

int  OfxTransactionContainer::add_to_main_tree()
{

  if(MainContainer != NULL)
    {
      return MainContainer->add_container(this);
    }
  else
    {
      return false;
    }
}


void OfxTransactionContainer::add_attribute(const string identifier, const string value)
{

  if(identifier=="DTPOSTED"){
    data.date_posted = ofxdate_to_time_t(value);
    data.date_posted_valid = true;
  }
  else if(identifier=="DTUSER"){
    data.date_initiated = ofxdate_to_time_t(value);
    data.date_initiated_valid = true;
  }
  else if(identifier=="DTAVAIL"){
    data.date_funds_available = ofxdate_to_time_t(value);
    data.date_funds_available_valid = true;
  }
  else if(identifier=="FITID"){
    strncpy(data.fi_id,value.c_str(), sizeof(data.fi_id));
    data.fi_id_valid = true;
  }
  else if(identifier=="CORRECTFITID"){
    strncpy(data.fi_id_corrected,value.c_str(), sizeof(data.fi_id));
    data.fi_id_corrected_valid = true;
  }
  else if(identifier=="CORRECTACTION"){
    data.fi_id_correction_action_valid=true;
    if(value=="REPLACE"){
      data.fi_id_correction_action=data.REPLACE;
    }
    else if(value=="DELETE"){
      data.fi_id_correction_action=data.DELETE;
    }
    else{
      data.fi_id_correction_action_valid=false;
    }
  }
  else if((identifier=="SRVRTID")||(identifier=="SRVRTID2")){
    strncpy(data.server_transaction_id,value.c_str(), sizeof(data.server_transaction_id));
    data.server_transaction_id_valid = true;
  }
  else if(identifier=="MEMO" || identifier=="MEMO2"){
    strncpy(data.memo,value.c_str(), sizeof(data.memo));
    data.memo_valid = true;
  }
  else{
    /* Redirect unknown identifiers to the base class */
    OfxGenericContainer::add_attribute(identifier, value);
  }
}// end OfxTransactionContainer::add_attribute()

void OfxTransactionContainer::add_account(OfxAccountData * account_data)
{
  if(account_data->account_id_valid==true)
    {
      data.account_ptr = account_data;
      strncpy(data.account_id,account_data->account_id,OFX_ACCOUNT_ID_LENGTH);
      data.account_id_valid = true;
    }
}

/***************************************************************************
 *                      OfxBankTransactionContainer                        *
 ***************************************************************************/

OfxBankTransactionContainer::OfxBankTransactionContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier): 
  OfxTransactionContainer(para_parentcontainer, para_tag_identifier)
{
  ;
}
void OfxBankTransactionContainer::add_attribute(const string identifier, const string value)
{
  if( identifier=="TRNTYPE"){
    data.transactiontype_valid=true;
    if(value=="CREDIT"){
      data.transactiontype=data.OFX_CREDIT;
    }
    else if(value=="DEBIT"){
      data.transactiontype=data.OFX_DEBIT;
    }
    else if(value=="INT"){
      data.transactiontype=data.OFX_INT;
    }
    else if(value=="DIV"){
      data.transactiontype=data.OFX_DIV;
    }
    else if(value=="FEE"){
      data.transactiontype=data.OFX_FEE;
    }
    else if(value=="SRVCHG"){
      data.transactiontype=data.OFX_SRVCHG;
    }
    else if(value=="DEP"){
      data.transactiontype=data.OFX_DEP;
    }
    else if(value=="ATM"){
      data.transactiontype=data.OFX_ATM;
    }
    else if(value=="POS"){
      data.transactiontype=data.OFX_POS;
    }
    else if(value=="XFER"){
      data.transactiontype=data.OFX_XFER;
    }
    else if(value=="CHECK"){
      data.transactiontype=data.OFX_CHECK;
    }
    else if(value=="PAYMENT"){
      data.transactiontype=data.OFX_PAYMENT;
    }
    else if(value=="CASH"){
      data.transactiontype=data.OFX_CASH;
    }
    else if(value=="DIRECTDEP"){
      data.transactiontype=data.OFX_DIRECTDEP;
    }
    else if(value=="DIRECTDEBIT"){
      data.transactiontype=data.OFX_DIRECTDEBIT;
    }
    else if(value=="REPEATPMT"){
      data.transactiontype=data.OFX_REPEATPMT;
    }
    else if(value=="OTHER"){
      data.transactiontype=data.OFX_OTHER;
    }
    else {
      data.transactiontype_valid=false;
    }
  }//end TRANSTYPE
  else if(identifier=="TRNAMT"){
    data.amount=ofxamount_to_double(value);
    data.amount_valid=true;
    data.units = -data.amount;
    data.units_valid=true;
    data.unitprice = 1,00;
    data.unitprice_valid=true;
  }
  else if(identifier=="CHECKNUM"){
    strncpy(data.check_number,value.c_str(), sizeof(data.check_number));
    data.check_number_valid = true;
  }
  else if(identifier=="REFNUM"){
    strncpy(data.reference_number,value.c_str(), sizeof(data.reference_number));
    data.reference_number_valid = true;
  }
  else if(identifier=="SIC"){
    data.standard_industrial_code = atoi(value.c_str());
    data.standard_industrial_code_valid = true;
  }
  else if((identifier=="PAYEEID")||(identifier=="PAYEEID2")){
    strncpy(data.payee_id,value.c_str(), sizeof(data.payee_id));
    data.payee_id_valid = true;
  }
  else if(identifier=="NAME"){
    strncpy(data.name,value.c_str(), sizeof(data.name));
    data.name_valid = true;
  }
  else{
    /* Redirect unknown identifiers to base class */
    OfxTransactionContainer::add_attribute(identifier, value);
  }
}//end OfxBankTransactionContainer::add_attribute
 

/***************************************************************************
 *                    OfxInvestmentTransactionContainer                    *
 ***************************************************************************/

OfxInvestmentTransactionContainer::OfxInvestmentTransactionContainer(OfxGenericContainer *para_parentcontainer, string para_tag_identifier):
  OfxTransactionContainer(para_parentcontainer, para_tag_identifier)
{
  type="INVESTMENT";
  data.transactiontype=data.OFX_OTHER;
  data.transactiontype_valid=true;
  
  data.invtranstype_valid=true; 
  strncpy(data.invtranstype,para_tag_identifier.c_str(),sizeof(data.invtranstype));
}

void OfxInvestmentTransactionContainer::add_attribute(const string identifier, const string value)
{
  if(identifier=="UNIQUEID"){
    strncpy(data.unique_id,value.c_str(), sizeof(data.unique_id));
    data.unique_id_valid = true;
  }
  else if(identifier=="UNIQUEIDTYPE"){
    strncpy(data.unique_id_type,value.c_str(), sizeof(data.unique_id_type));
    data.unique_id_type_valid = true;
  }
  else if(identifier=="UNITS"){
    data.units=ofxamount_to_double(value);
    data.units_valid = true;
  }
  else if(identifier=="UNITPRICE"){
    data.unitprice=ofxamount_to_double(value);
    data.unitprice_valid = true;
  }
  else if(identifier=="TOTAL"){
    data.amount=ofxamount_to_double(value);
    data.amount_valid = true;
  }
  else if(identifier=="DTSETTLE"){
    data.date_posted = ofxdate_to_time_t(value);
    data.date_posted_valid = true;
  }
  else if(identifier=="DTTRADE"){
    data.date_initiated = ofxdate_to_time_t(value);
    data.date_initiated_valid = true;
  }
  else{
    /* Redirect unknown identifiers to the base class */
    OfxTransactionContainer::add_attribute(identifier, value);
  }
}//end OfxInvestmentTransactionContainer::add_attribute
