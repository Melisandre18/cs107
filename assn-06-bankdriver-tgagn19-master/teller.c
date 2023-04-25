#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

#include "teller.h"
#include "account.h"
#include "error.h"
#include "debug.h"
#include "branch.h"
/*
 * deposit money into an account
 */
int
Teller_DoDeposit(Bank *bank, AccountNumber accountNum, AccountAmount amount)
{
  assert(amount >= 0);

  DPRINTF('t', ("Teller_DoDeposit(account 0x%"PRIx64" amount %"PRId64")\n",
                accountNum, amount));

  Account *account = Account_LookupByNumber(bank, accountNum);
  BranchID ID = AccountNum_GetBranchID(accountNum);

  sem_wait(&(account->acc));
  sem_wait(&(bank->branches[ID].lock));
  if (account == NULL) {
    sem_post(&(account->acc));
    sem_post(&(bank->branches[ID].lock));
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  Account_Adjust(bank,account, amount, 1);
  
  sem_post(&(account->acc));
  sem_post(&(bank->branches[ID].lock));
  return ERROR_SUCCESS;
}

/*
 * withdraw money from an account
 */
int
Teller_DoWithdraw(Bank *bank, AccountNumber accountNum, AccountAmount amount)
{
  assert(amount >= 0);

  DPRINTF('t', ("Teller_DoWithdraw(account 0x%"PRIx64" amount %"PRId64")\n",
                accountNum, amount));

  Account *account = Account_LookupByNumber(bank, accountNum);
  BranchID ID = AccountNum_GetBranchID(accountNum);

  sem_wait(&(account->acc));
  sem_wait(&(bank->branches[ID].lock));

  if (account == NULL) {
    sem_post(&(account->acc));
    sem_post(&(bank->branches[ID].lock));

    return ERROR_ACCOUNT_NOT_FOUND;
  }

  if (amount > Account_Balance(account)) {

    sem_post(&(account->acc));
    sem_post(&bank->branches[ID].lock);

    return ERROR_INSUFFICIENT_FUNDS;
  }

  Account_Adjust(bank,account, -amount, 1);
  sem_post(&(account->acc));
  sem_post(&(bank->branches[ID].lock));
  return ERROR_SUCCESS;
}

/*
 * do a tranfer from one account to another account
 */
int
Teller_DoTransfer(Bank *bank, AccountNumber srcAccountNum,
                  AccountNumber dstAccountNum,
                  AccountAmount amount)
{
  assert(amount >= 0);

  DPRINTF('t', ("Teller_DoTransfer(src 0x%"PRIx64", dst 0x%"PRIx64
                ", amount %"PRId64")\n",
                srcAccountNum, dstAccountNum, amount));

  Account *srcAccount = Account_LookupByNumber(bank, srcAccountNum);
  if (srcAccount == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  Account *dstAccount = Account_LookupByNumber(bank, dstAccountNum);
  if (dstAccount == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  if(srcAccount->accountNumber== dstAccount->accountNumber) return ERROR_SUCCESS;
    if(Account_IsSameBranch(srcAccount,dstAccount)){
      if(srcAccount->accountNumber<dstAccount->accountNumber){
        sem_wait(&(srcAccount->acc));
        sem_wait(&(dstAccount->acc));
      }else{
        sem_wait(&(dstAccount->acc));
        sem_wait(&(srcAccount->acc));
      }
   if (amount > Account_Balance(srcAccount)) {
      sem_post(&(srcAccount->acc));
      sem_post(&(dstAccount->acc));
    return ERROR_INSUFFICIENT_FUNDS;
  }

  /*
   * If we are doing a transfer within the branch, we tell the Account module to
   * not bother updating the branch balance since the net change for the
   * branch is 0.
   */
  //int updateBranch = !Account_IsSameBranch(srcAccountNum, dstAccountNum);

  Account_Adjust(bank, srcAccount, -amount, 0);
  Account_Adjust(bank, dstAccount, amount, 0);
  sem_post(&(srcAccount->acc));
  sem_post(&(dstAccount->acc));
  return ERROR_SUCCESS;
    }else{
       int sId=AccountNum_GetBranchID(srcAccountNum);
       int dId=AccountNum_GetBranchID(dstAccountNum);
       if(sId<dId){
          sem_wait(&(srcAccount->acc));
          sem_wait(&(dstAccount->acc));
          sem_wait(&(bank->branches[sId].lock));
          sem_wait(&(bank->branches[dId].lock));
       }else{
          sem_wait(&(dstAccount->acc));
          sem_wait(&(srcAccount->acc));
          sem_wait(&(bank->branches[sId].lock));
          sem_wait(&(bank->branches[sId].lock));
       }

      if (amount > Account_Balance(srcAccount)) {
      sem_post(&(dstAccount->acc));
      sem_post(&(srcAccount->acc)); 
      sem_post(&(bank->branches[dId].lock));
      sem_post(&(bank->branches[sId].lock));

      return ERROR_INSUFFICIENT_FUNDS;
    }
      Account_Adjust(bank, srcAccount, -amount, 1);
      Account_Adjust(bank, dstAccount, amount, 1);

      sem_post(&(dstAccount->acc));
      sem_post(&(srcAccount->acc)); 
      sem_post(&(bank->branches[dId].lock));
      sem_post(&(bank->branches[sId].lock));

    return ERROR_SUCCESS;
    }
}
