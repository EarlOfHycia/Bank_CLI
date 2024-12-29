// if time permits implement a live writing terminal program/function....
//add delays everywhere XD
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

string global_date;


// classes are separated by ///////////////////////// for good readability

// this function return interest as a decimal and not as a percent ie return values are from 0 to 1 and not from 0 to 100
double interest_function(double amount, double account_type){
    if(account_type==0){
        return 0;
    }
    else{
        if(amount<100000){
            return 0.02;
        }
        if(amount<1000000){
            return 0.04;
        }
        else{
            return 0.06;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class transaction_hist_entry{
    friend class bank_account;
    friend class bank_account_holder;
public:
    string transaction_type;
    double transaction_amount;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class bank_account{
    
    friend class branch_manager;
    friend class bank_account_holder;
public:
    string open_date;
    long long int account_number;
    double balance;
    double interest_rate;
    double type;
    vector<transaction_hist_entry*>transaction_history;
    int number_of_transactions = 0;
    bank_account(){}
    
    void deposit(double deposit_amount){
        if(deposit_amount>0){
            balance += deposit_amount;
            cout<<deposit_amount<<" has been deposited into account "<<account_number<<"\n";
            transaction_hist_entry* ptr = new transaction_hist_entry;
            ptr->transaction_amount = deposit_amount;
            ptr->transaction_type = "Deposited";
            transaction_history.push_back(ptr);
            number_of_transactions++;
            interest_rate = interest_function(balance,type);
        }
        else{
            cout<<"Cannot deposit non-positive amount\n";
        }
        return;
    }
    
    void withdraw(double withdraw_amount){
        if(withdraw_amount<=0){
            cout<<"Cannot withdraw non-positive amount\n";
        }
        else if(balance < withdraw_amount){
            cout<<"Insufficient Balance\n";
        }
        else{
            if(type && withdraw_amount>10000){
                cout<<"\nWithdrawl denied..";
                cout<<"\nAmount requested to withdraw is greater than the transaction limit of 10000\n";
            }
            else{
                balance -= withdraw_amount;
                cout<<withdraw_amount<<" has been withdrawn from account "<<account_number<<"\n";
                transaction_hist_entry* ptr = new transaction_hist_entry;
                ptr->transaction_amount = withdraw_amount;
                ptr->transaction_type = "Withdrawn";
                transaction_history.push_back(ptr);
                number_of_transactions++;
                interest_rate = interest_function(balance,type);
            }
            
        }
        return;
    }
   
    void transfer(bank_account* to_account, double transfer_amount){
        if(transfer_amount < 0 ){
            cout<<"Cannot transfer negative amount\n";
        }
        else if(balance < transfer_amount){
            cout<<"Insufficient Balance to perform transfer\n";
        }
        else{
            if(type && transfer_amount>10000){
                cout<<"\nTransfer denied";
                cout<<"\nAmount requested to transfer is greater than the transaction limit of 10000\n";
            }
            else{
                balance -= transfer_amount;
                to_account->balance += transfer_amount;
                interest_rate = interest_function(balance,type);
                to_account->interest_rate = interest_function(to_account->balance,to_account->type);
                cout<<transfer_amount<<" has been transfered from account "<<account_number<<" to account "<<to_account->account_number<<"\n";
                
                transaction_hist_entry* ptr = new transaction_hist_entry;
                ptr->transaction_amount = transfer_amount;
                ptr->transaction_type = "Transfered";
                transaction_history.push_back(ptr);
                number_of_transactions++;
                to_account->number_of_transactions +=1;
                transaction_hist_entry* ptr2 = new transaction_hist_entry;
                ptr2->transaction_amount = transfer_amount;
                ptr2->transaction_type = "Received";
                to_account->transaction_history.push_back(ptr2);
            }
            
        }
    }

    void get_statements(){
        string str = (type)?"Savings":"Checking";
        cout<<"\nTransaction History for "<< str <<" account "<<account_number<<"\n\n";
        for(int i=0;i<number_of_transactions;i++){
            cout<<transaction_history[i]->transaction_amount<<" "<<transaction_history[i]->transaction_type<<" on "<<global_date<<"\n";
        }
        return;
    }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// link bank_account and bank_account_holder----------done
// done using bank_account* ptr in bank_account_holder class

class bank_account_holder{
    friend class branch_manager;
public:
    vector<bank_account*>bank_accounts_list;
    int number_of_accounts = 0;
    string name;
    string user_name;
    string net_banking_password;
    bank_account_holder(string s, string us, string nbp){
        name = s;
        user_name = us;
        net_banking_password = nbp;
       }
    void get_accounts(){
        cout<<"\nCustomer "<<name<<" has the following accounts:\n";
        cout<<"Account No. Type  Balance\n";
        for(int i=0;i<number_of_accounts;i++){
            string str =bank_accounts_list[i]->type?"Savings":"Checking";
            cout<<"\t\t"<<bank_accounts_list[i]->account_number<<"\t"<<str<<" "<<bank_accounts_list[i]->balance<<"\n";
        }
        return;
    }
    void change_password(string new_password){
        net_banking_password = new_password;
    }
        bank_account* create_account(long long int new_acc_num,string op_date,double deposit){
        bank_account* temp = new bank_account;
        temp->account_number = new_acc_num;
        temp->open_date = op_date;
        temp->balance = deposit;
        transaction_hist_entry* hist = new transaction_hist_entry;
        hist->transaction_amount = deposit;
        hist->transaction_type = "Deposited On Account Creation";
        temp->transaction_history.push_back(hist);
        temp->number_of_transactions++;
        bank_accounts_list.push_back(temp);
        number_of_accounts++;
        return temp;
    }
};


vector<bank_account_holder*> list_of_account_holders;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class savings_account:bank_account{
    friend class branch_manager;
    friend class bank_account_holder;
public:
    double interest_rate;

    savings_account(){}
    
    double get_interest_rate(){
        return interest_rate;
    }

    void set_interest_rate(double rate){
        interest_rate = rate;
    }
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class checking_account:bank_account{
    friend class branch_manager;
    friend class bank_account_holder;
    double interest_rate = 0.0;
public:
    checking_account(){}
    void get_interest_rate(){
        }
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///

//finds account from the entire database
bank_account* find_account(int acc_num){
    int num = list_of_account_holders.size();
    for(int i=0;i<num;i++){
        int n = list_of_account_holders[i]->number_of_accounts;
        for(int j=0;j<n;j++){
            if(acc_num==list_of_account_holders[i]->bank_accounts_list[j]->account_number){
                return list_of_account_holders[i]->bank_accounts_list[j];
            }
        }
    }
    return NULL;
}

class branch_manager{
    
    static branch_manager* self;
    
    branch_manager(){}
public:
    static string username;
    static string password;
    static branch_manager* instance(){
        if(self==NULL){
            self = new branch_manager;
        }
        return self;
    }
    
    void fast_forward(){
        double days;
        cout<<"\n\nHow many days would like to fast forward?---->";
        cin>>days;
        int num = list_of_account_holders.size();
        for(int i=0;i<num;i++){
            int n = list_of_account_holders[i]->number_of_accounts;
            for(int j=0;j<n;j++){
                
                    double interest = list_of_account_holders[i]->bank_accounts_list[j]->balance*list_of_account_holders[i]->bank_accounts_list[j]->interest_rate *list_of_account_holders[i]->bank_accounts_list[j]->type * days / 365;
                    
                list_of_account_holders[i]->bank_accounts_list[j]->balance += interest;
                
                    list_of_account_holders[i]->bank_accounts_list[j]->interest_rate = interest_function(list_of_account_holders[i]->bank_accounts_list[j]->balance,list_of_account_holders[i]->bank_accounts_list[j]->type);
                
                if(list_of_account_holders[i]->bank_accounts_list[j]->type){
                    transaction_hist_entry* hist = new transaction_hist_entry;
                    hist->transaction_amount = interest;
                    hist->transaction_type = "Received as interest from Bank";
                    list_of_account_holders[i]->bank_accounts_list[j]->transaction_history.push_back(hist);
                }
            }
        }
        cout<<"\nFast Forward Done!!\nAll Accounts Have Been Updated..";
    }
    
    //option 2 get all accounts info
    void branch_manager_get_accounts_info(){
        cout<<"\n\nThese are the current operational accounts in DBank:";
        int num = list_of_account_holders.size();
        for(int i=0;i<num;i++){
            list_of_account_holders[i]->get_accounts();
        }
    }
    
    //option 1 Get_account Statement of any account
    void branch_manager_statement_access(){
        cout<<"\n\nWhich account would you like get statement of?";
        cout<<"\n\nEnter Account Number---->";
        int acc_num;
        cin>>acc_num;
        bank_account* account = find_account(acc_num);
        
        if(account){
            account->get_statements();
            
        }
        else{
            cout<<"\nAccount Does Not Exist!!";
        }
    }

};

branch_manager* branch_manager::self = NULL;
string branch_manager::username = "dny";
string branch_manager::password = "123";
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//TODO
// uniqueness determining functions

int unique_user_name(string user_name){
    int num = list_of_account_holders.size();
    for(int i=0;i<num;i++){
       if(!user_name.compare(list_of_account_holders[i]->user_name))
           return 0;
    }
    return 1;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// code of CLI options
bank_account_holder* signup(){
    cout<<"Welcome to the Signup Portal...\n";
    cout<<"\nEnter your name---->";
    string name;
    getline(cin,name);
    getline(cin,name);
    //add sleep_for
    string user_name;
    cout<<"\nChoose Your Username---->";
    getline(cin,user_name);
    while(!unique_user_name(user_name)){
        cout<<"\nUsername Already Taken\nPlease Choose a diiferent Username-->";
        getline(cin,user_name);
    }
    string passwd1,passwd2;
    cout<<"\nChoose Your Password------>";
    getline(cin,passwd1);
    cout<<"\nRe-Enter Your Password----->";
    getline(cin,passwd2);
    while(passwd1.compare(passwd2)){
        cout<<"\n\nPasswords Don't Match. Please Retry\n";
        cout<<"\nChoose Your Password------>";
        getline(cin,passwd1);
        cout<<"\nRe-Enter Your Password----->";
        getline(cin,passwd2);
    }
    cout<<"\n\nAccount Created Successfully!\n\n";
    bank_account_holder* temp = new bank_account_holder(name,user_name,passwd1);
    return temp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//try_login and subfunctions
//getting test_login info
bank_account_holder* try_login(){
    //complete this shit
    cout<<"DBank Account Login Page...\n";
    string test_name;
    cout<<"\nName-------->";
    getline(cin,test_name);
    getline(cin,test_name);
    //add delay;
    string test_user_name;
    cout<<"\nUsername---->";
    getline(cin,test_user_name);
    //add delay;
    string test_password;
    cout<<"\nPassword--->";
    getline(cin,test_password);
    
    
    bank_account_holder* temp = new bank_account_holder(test_name,test_user_name,test_password);
    return temp;
}

//verify_login checking with existing accounts info
bank_account_holder* verify_login(bank_account_holder* test, vector<bank_account_holder*> list){    

    int r1,r2,r3;
    int num = list_of_account_holders.size();
    for(int i=0;i<num;i++){
        r1 = test->name.compare(list[i]->name);
        if(r1) continue;
        r2 = test->user_name.compare(list[i]->user_name);
        if(r2) continue;
        r3 = test->net_banking_password.compare(list[i]->net_banking_password);
        if(r3) continue;
        return list[i];
    }
    return NULL;
}
// bank_account_login_get_choice successful login action input and subfunctions
int bank_account_login_get_choice(){
    cout<<"\n\nSuccessful Login!";
    cout<<"\n\nWhat would you like to do?...";
    cout<<"\n\n1. Create an Account";
    cout<<"\n2. Get Account(s) Information";
    cout<<"\n3. Change Account Password";
    cout<<"\n4. Do Transaction";
    cout<<"\n5. Get Transaction History";
    cout<<"\n6. Delete an Account";
    cout<<"\n\nEnter Your Choice (Number)--->";
    int choice;
    cin>>choice;
    return choice;
}
//option 1 account creation
bank_account* create_bank_account(bank_account_holder* holder,int* num){
    cout<<"\n\nWelcome to the Bank Account Creation Portal";
    cout<<"\n\nWhat kind of Account do you want to create?";
    cout<<"\n\n1. Savings Account";
    cout<<"\n2. Checking Account";
    cout<<"\n\nEnter Your Choice---->";
    int choice;
    cin>>choice;
    cout<<"\n\nA minimum deposit of 2000.00 is required for creating an account";
    cout<<"\nHow much amount would you like to deposit?--->";
    double test_deposit;
    cin>>test_deposit;
    if(test_deposit>=2000){
        (*num)++;
        bank_account* account = holder->create_account(*num, global_date, test_deposit);
        if(choice==1){
            account->type=1;
            account->interest_rate = interest_function(test_deposit,1);
        }
        if(choice==2){
            account->type=0;
            account->interest_rate=0;
        }
        cout<<"\nAccount Creation Successful..";
        return account;
    }
    
    else{
        cout<<"\nAccount Creation Failed..";
        return NULL;
    }
}
//option 2 get account info
void get_account_infos(bank_account_holder* holder){
    cout<<"\n";
    holder->get_accounts();
}

//option 3 change account password
void change_account_password(bank_account_holder* holder){
    string passwd1,passwd2;
    cout<<"\n\nEnter New Password------>";
    getline(cin,passwd1);
    getline(cin,passwd1);
    cout<<"\nRe-Enter New Password----->";
    getline(cin,passwd2);
    while(passwd1.compare(passwd2)){
        cout<<"\n\nPasswords Don't Match. Please Retry\n";
        cout<<"\nChoose Your Password------>";
        getline(cin,passwd1);
        cout<<"\nRe-Enter Your Password----->";
        getline(cin,passwd2);
    }
    cout<<"\n\nPassword Updated Successfully!!";
    holder->net_banking_password = passwd1;
    
}

//option 4  find_account, which_account and do_transaction


// finds account of specific account holder
bank_account* which_account(bank_account_holder* holder){
    cout<<"\n\nWhich account do you want to perform action on?";
    cout<<"\nEnter Account Number--->";
    int acc_num;
    cin>>acc_num;
    
    int num = holder->number_of_accounts;
    
    for(int i=0;i<num;i++){
        if(acc_num == holder->bank_accounts_list[i]->account_number){
            return holder->bank_accounts_list[i];
        }
    }
    return NULL;
}

//does transaction
void do_transaction(bank_account* account){
    if(!account) return;
    
    cout<<"\n\nWhat would you like to do?..";
    cout<<"\n1. Deposit";
    cout<<"\n2. Withdraw";
    cout<<"\n3. Transfer";
    cout<<"\n\n Enter Your Choice (Number)--->";
    int choice;
    cin>>choice;
    
    //deposit
    if(choice==1){
        cout<<"\n\nHow much amount would you like to deposit?..";
        double deposit;
        cin>>deposit;
        account->deposit(deposit);
    }
    
    //withdraw
    if(choice==2){
        cout<<"\n\nHow much amount would you like to withdraw?..";
        double withdraw;
        cin>>withdraw;
        account->withdraw(withdraw);
    }
    
    //transfer
    if(choice==3){
        cout<<"\n\nTo which account would you like to transfer money?";
        cout<<"\nEnter Account Number---->";
        int acc_num;
        cin>>acc_num;
        bank_account* to = find_account(acc_num);
        if(!to) {
            cout<<"\n\nThis Account Does Not Exist!!";
            return;
        }
        cout<<"\n\nHow much amount would you like to transfer?..";
        double transfer;
        cin>>transfer;
        account->transfer(to, transfer);
    }
}

//option 5 get transaction history
void get_transaction_history(bank_account_holder* holder){
    bank_account* account = which_account(holder);
    if(account){
        account->get_statements();
    }
    else{
        cout<<"\n\nThis Account Does Not Exist!!";
    }
}

//option 6 delete an account
void delete_bank_account(bank_account_holder* holder,bank_account* account){
    auto it = find(holder->bank_accounts_list.begin(),holder->bank_accounts_list.end(),account);
    
    if(it != holder->bank_accounts_list.end()){
        holder->bank_accounts_list.erase(it);
    }

}

// unsuccessful login
void failed_login(){
    cout<<"\n\nLogin Failed!!\nProvided Credentials dont't match to any account\nPlease Try Again..";
}

//Branch Manager Login Stuff
int branch_manager_try_login(){
    cout<<"\nEnter Username--->";
    string test_user_name,test_password;
    getline(cin,test_user_name);
    getline(cin,test_user_name);
    cout<<"\nEnter Password--->";
    getline(cin,test_password);
    
    int r1,r2;
    
    r1 = branch_manager::username.compare(test_user_name);
    if(r1){
        return 0;
    }
    r2 = branch_manager::password.compare(test_password);
    if(r2){
        return 0;
    }
    return 1;
}
//Branch Manager Dashboard
int branch_manager_login_page(){
    cout<<"\n\nWelcome Branch Manager..";
    cout<<"\n\nWhat would you like to do?";
    cout<<"\n\n1. Get statement of an Account";
    cout<<"\n2. Get Account Holders' Information";
    cout<<"\n3. Fast Forward";
    cout<<"\n\nEnter Your Choice (Number)---->";
    int choice;
    cin>>choice;
    return choice;
}

//branch manager functions and subfunctions



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(){

    //branch_manager instance creation
    branch_manager::instance();
    
    int number_of_bank_accounts=0;

    cout<<"\n\nWelcome to DBank Command Line Interface...\n\n";
    cout<<"Please Enter Today's Date in DD/MM/YYYY format\n";
    cout<<"Date---->";
    cin>>global_date;
    while(1){
        cout<<"\n\nWhat would you like to do?\n";
        cout<<"1. Account Holder Signup\n";
        cout<<"2. Account Holder Login\n";
        cout<<"3. Branch Manager Login\n";
        cout<<"4. Exit\n\n";
        
        cout<<"Enter Your Choice (Number)--->";
        int choice;
        cin>>choice;
        cout<<"\n\n";
        
        if(choice==4){
            cout<<"Thank You For Your Visit!!\n";
            break;
        }
        
        //for signup
        if(choice==1){//add sleep_for
            list_of_account_holders.push_back(signup());
        }
        
        //for login
        if(choice==2){
            bank_account_holder* test_account_holder = try_login();
            bank_account_holder* result_account_holder = verify_login(test_account_holder,list_of_account_holders);
            if(result_account_holder){//for successful login
                int choice = bank_account_login_get_choice();
                //for account creation
                if(choice==1){
                    create_bank_account(result_account_holder, &number_of_bank_accounts);
                }
                
                //for getting account info
                if(choice==2){
                    get_account_infos(result_account_holder);
                }
                
                //for changing account password
                if(choice==3){
                    change_account_password(result_account_holder);
                }
                
                //for doing transaction
                if(choice==4){
                    bank_account* account = which_account(result_account_holder);
                    if(account) do_transaction(account);
                    else cout<<"\n\nAccount Does Not Exist!!";
                }
                
                //for getting transaction history
                if(choice==5){
                    get_transaction_history(result_account_holder);
                }
                
                //for deleting account
                if(choice==6){
                    bank_account* account = which_account(result_account_holder);
                    if(account){
                        cout<<"\nDeletion of an Account is an irrevesible action..";
                        cout<<"\nAre you sure you want to continue?";
                        cout<<"\n\nType Y for Yes and N for No--->";
                        char c;
                        cin>>c;
                        if(c=='Y'){
                            delete_bank_account(result_account_holder,account);
                            delete account;
                            cout<<"\nAccount has been deleted successfuly..";
                        }
                        if(c=='N') cout<<"\nDeletion Process has been canceled";
                        else cout<<"\nInvalid Input";
                    }
                    else cout<<"\n\nAccount Does Not Exist!!";
                }
                
            }
            
            else{
                //for unsuccessful login
                failed_login();
            }
        }
        
        //for branch_manager_login
        if(choice==3){
            int result = branch_manager_try_login();
            if(result){ 
               int choice = branch_manager_login_page();
                
                //get account statement
                if(choice==1){
                    branch_manager::instance()->branch_manager_statement_access();
                }
                
                //get account holders info
                if(choice==2){
                    branch_manager::instance()->branch_manager_get_accounts_info();
                }
                
                //fast_forward funtion
                if(choice==3){
                    branch_manager::instance()->fast_forward();
                }
            }
            else cout<<"\n\nLogin Failed!!\nIncorrect Credentials";
            
        }
        //implement this
        //youre being redirected to the home page
    }
    return 0;
}
