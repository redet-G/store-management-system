#include <iostream>
#include <iomanip>
#include <conio.h>
#include <fstream>
#include <cstdlib>//system function
#include <string.h>
#include <limits>//for get_num function for finding  the limit of stream size.
using namespace std;

enum  status {AVAILABLE,ISSUED,DISPATCHED,UNKNOWN};
char status_value[4][11]= {"AVAILABLE","ISSUED","DISPATCHED","UNKNOWN"};

struct pkg//short for package
{
    int serial_no;
    char  model[10];
    char description[50];
    int quantity;
    double price;
    status stat;
} current_store[1000];
int current_pkg_num=0;


void process();
int yes_no();
int load(int status_bar=0);//status_bar = 1(true) to show
int write();
int search_by_model(char [],status s=UNKNOWN);
void issue(char []);
void display(int index=-1);
int report();
int main_menu();
void export_file();
void export_report();
void display_report();
void about();
int is_empty_string(char []);
int get_num_inst();
double get_num(char type='d');
void register_pkg();
int store_menu();
void help();
void greet();
int run();


int main()
{
    system("mode 120,1200&&color f0&&title Departmental Store Management System");//set window size big enough to show the necessary informations and set window title
    load(1);//loading the store form the file
    greet();
    cout<<setprecision(2);//round the floating numbers to the 2 nearest decimal place
    cout<<fixed;//set the output stream fixed floating number notation
    while(run());//start the flow control function called run
    return 0;
}

/***********************************************************************
    this function will get only number from the user and if a user
    entered a char or a string it will ask the user to enter again
    it has a default parameter type = 'd' for double and 'i' for integer
    it will check for over of the variables based on the type specified
    in the parameter.
************************************************************************/
double get_num(char type)
{
    double temp;
    while(!(cin>>temp))
    {
        cout<<"invalid input:(non-numeric value given) please try again"<<endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
    }
    cin.ignore();
    if(temp<0)
    {
        cout<<"error: invalid number. can not handle too big or negative number."<<endl<<"please try again"<<endl;
        return get_num();
    }
    if(temp>numeric_limits<int>::max()&&type=='i')
    {
        cout<<"error: invalid number. can not handle too big or negative number."<<endl<<"please try again"<<endl;
        return get_num('i');
    }
    return temp;
}
/*********************************************************************
    this function will get a char form the keyboard whenever it is
    pressed and returns it by converting to integer
**********************************************************************/
int get_num_inst()
{
    char c;
    for(;;)
    {
        if(_kbhit())
        {
            c=getch();
            return (c<58&&c>47)?(c-48):-1;//if the char is a number return the number else return -1
        }
    }
}


/****************************************************************************************
    this function will accept inputs form the user and only return boolean(1 or 0)
    if the user pressed y/Y for yes or n/N for n. true(1) for the former and false(0)
    for later
*****************************************************************************************/
int yes_no()
{
    //get the user input (yes or no)
    char y_n;
    do
    {
        y_n=getch();
    }
    while(!(y_n=='y'||y_n=='n'||y_n=='Y'||y_n=='N'));
    if(y_n=='y'||y_n=='Y')
        return 1;//yes
    else
        return 0;//no
}


/*********************************************************************************
    this function will return a boolean(int), true(1) if the string is empty i.e
    either it is full of space characters or contains no characters.
**********************************************************************************/
int is_empty_string(char c[])
{
    int n=strlen(c);
    int space=0;
    if(n<2&&c[0]==' ')
    {
        return 1;
    }
    else
    {
        for (int i=0; i<n; i++)
        {
            if(c[i]==' ')
                space++;
        }
        if(space==n)
            return 1;
    }
    return 0;//it is not an empty string
}

/***********************************************************************
    this function registers a package and has the following features
            1)it search a package by its model by using search_by_model()
                function and if the package is found it will substitute all
                the details form the the package that was found to be identical
                ask for additional quantity to be registered.
            2)used the process function to process and save the packages in to the
                file
            3)and finally ask for more registration
************************************************************************/
void register_pkg()
{
    pkg property;
    cout<<"	        REGISTRATION/IMPORTING FORM"<<endl;
    cout<<"================================================"<<endl;

    do
    {

        cout<<"model:";
        cin.getline(property.model,10);
        while(is_empty_string(property.model))
        {
            cout<<"model can not be an empty string. please try again."<<endl;
            cin.getline(property.model,10);
        }

        //store the search result by searching for package by its model and status
        int result=search_by_model(property.model,ISSUED);
        if(result==-1)
        {
            //search the package only by its model and store it
            int search_result=search_by_model(property.model);
            if(search_result==-1)
            {
                cout<<"serial no:";
                property.serial_no=get_num('i');//get integer number
                cout<<"detail description :";
                cin.getline(property.description,50);

                while(is_empty_string(property.description))
                {
                    cout<<"a description can not be an empty string. please try again."<<endl;
                    cin.getline(property.description,10);
                }
                cout<<"quantity: ";
                property.quantity=get_num('i');
                cout<<"unit price: ";
                property.price=get_num();
            }
            else
            {
                cout<<"the model already exits. enter the quantity you want to import to the store"<<endl;
                display(search_result);
                property=current_store[search_result];
                cout<<"quantity: ";
                property.quantity=get_num('i');
            }
            property.stat = AVAILABLE;
            current_store[current_pkg_num++]=property;
        }
        else
        {
            cout<<"issued package is found with this model!"<<endl<<"enter the quantity you want to import to the store"<<endl;
            display(result);
            property=current_store[result];
            cout<<"quantity: ";
            property.quantity=get_num('i');

            if(property.quantity<current_store[result].quantity)
            {
                current_store[result].quantity-=property.quantity;
                cout<<"WARNING: the full issued quantity was not satisfied."<<endl;
                write();//save
                display(result);
                property.stat = AVAILABLE;
                current_store[current_pkg_num++]=property;
                getche();
            }
            else
            {
                cout<<"INFO: the issued quantity has been satisfied successfully."<<endl;
                //replace the issued recored with the current imported package
                current_store[result]=property;
                getche();
            }


        }
        process();
        cout<<"do you went to continue the registering?(y/n)"<<endl;
    }
    while(yes_no());
}
/***********************************************************************
    this function displays
            1)the whole store package if it is called with no argument
            2)the indexed package only if it is called with only one argument
            3)the packages starting from index to index+num if it is called with two arguments

************************************************************************/
void display(int index)
{

    if(load())
    {
        if(index==-1)cout<<" DISPLAYING PROPERTY LIST"<<endl;
        cout<<" "<<setfill('_')<<setw(98)<<"_"<<" "<<setfill(' ')<<endl;
        cout<<"|"<<left<<setw(9)<<"serial no. "<<setw(10)<<"|model"<<setw(25)<<"|description"<<setw(10)<<"|quantity"<<"|"<<setw(20)<<right<<"unit price "<<"|"<<setw(20)<<left<<"status "<<"|"<<endl;
        cout<<setfill('=')<<setw(12)<<"|"<<setw(10)<<"|"<<setw(25)<<"|"<<setw(10)<<"|"<<setw(21)<<"|"<<setw(21)<<"|"<<"|"<<setfill(' ')<<endl;

        if(index==-1)
        {
            for(int i=0; i<current_pkg_num; i++)
            {
                cout<<left<<"|"<<setw(11)<<current_store[i].serial_no<<"|"<<setw(9)<<current_store[i].model<<"|"<<setw(24)<<current_store[i].description<<"|"<<setw(9)<<current_store[i].quantity<<"|"<<setw(20)<<right<<current_store[i].price<<"|"<<setw(20)<<right<<status_value[(int)current_store[i].stat]<<"|"<<endl;
            }
            cout<<"|"<<setfill('_')<<setw(12)<<"|"<<setw(10)<<"|"<<setw(25)<<"|"<<setw(10)<<"|"<<setw(21)<<"|"<<setw(21)<<"|"<<setfill(' ')<<endl;
            getch();
        }
        else
        {
            cout<<left<<"|"<<setw(11)<<current_store[index].serial_no<<"|"<<setw(9)<<current_store[index].model<<"|"<<setw(24)<<current_store[index].description<<"|"<<setw(9)<<current_store[index].quantity<<"|"<<setw(20)<<right<<current_store[index].price<<"|"<<setw(20)<<right<<status_value[(int)current_store[index].stat]<<"|"<<endl;
            cout<<"|"<<setfill('_')<<setw(12)<<"|"<<setw(10)<<"|"<<setw(25)<<"|"<<setw(10)<<"|"<<setw(21)<<"|"<<setw(21)<<"|"<<setfill(' ')<<endl;
        }

    }
    else
    {
        cout<<"ERROR: cannot display recored(s)"<<endl;
    }

}

/**********************************************************************
 A function for loading the packages on the file to the current store
**********************************************************************/
int load(int status_bar) //status_bar = 1(true) to show status bar
{
    int num=0,bar=0,temp=0;

    current_pkg_num=0;//delete the current store


    ifstream reader("bin_db.dat",ios::binary);

    if(reader.fail())
    {
        if(!status_bar)
        {
            cout <<"error: loading failed because the file is not found."<<endl;
            getch();
        }
        return 0;//for indication that the loading was not successful
    }
    pkg property;
    if(status_bar){
    ifstream filesize("bin_db.dat",ios::ate|ios::binary);
    bar=filesize.tellg()/sizeof(property);
    filesize.close();
    cout<<"loading ..."<<endl;

    }
    while(!reader.eof()||reader.good())
    {
        reader.read((char*)&property,sizeof(property));

        if(reader.eof())break;//to break out of the loop if the next pkg not available
        current_store[current_pkg_num++]=property;

        //loading bar
        if(status_bar&&!reader.eof())
        {
           if(((num*100/bar)-temp)>0.5){
                cout<<"#";
                temp=num*100/bar;
            }
            num++;

        }//end of loading bar

    }
    reader.close();

    return 1;//for indicating the successful completion loading operation
}
/**********************************************************************
  A function for writing the packages on the current store to the file
***********************************************************************/
int write()
{
    ofstream binfile("bin_db.dat",ios::binary|ios::out);

    if(binfile.fail())
    {
        cout <<"ERROR: there was an error writing on the file";
        return 0;//for indication that the loading was not successful
    }
     for(int i=0; i<current_pkg_num; i++)
    {
        binfile.write((char*)&current_store[i],sizeof(current_store[i]));
    }
    binfile.close();
    return 1;
}

/**************************************************
   a function that searches for package by mainly
   model and optionally by their status it returns
   the index number of the current store. if the
   package if found and return -1.
***************************************************/
int search_by_model(char model[],status s)
{
    for(int i=0; i<current_pkg_num; i++)
    {
        if(strcmp(current_store[i].model,model)==0)
        {
            if(s==UNKNOWN)
                return i;//model found
            else if(current_store[i].stat==s)
                return i;

        }
    }
    return (-1);//model is not found
}

/*************************************************************
    shows a menu and get the user selection and return it
**************************************************************/
int store_menu()
{
    system("cls");

    cout<<setfill('_')<<setw(56)<<" "<<endl<<left;
    cout<<setfill(' ')<<setw(56)<<"| STORE MENU"<<"|"<<endl;
    cout<<setw(50)<<"|	1)register/import property"<<"|"<<endl;
    cout<<setw(50)<<"|	2)display property"<<"|"<<endl;
    cout<<setw(50)<<"|	3)dispatch property"<<"|"<<endl;
    cout<<setw(50)<<"|	4)issue property"<<"|"<<endl;
    cout<<setw(50)<<"|	5)process and sort the store"<<"|"<<endl;
    cout<<setw(50)<<"|	6)REPORT MENU"<<"|"<<endl;
    cout<<setw(50)<<"|	7)export property list to other file format"<<"|"<<endl;
    cout<<setw(50)<<"|	0)back to MAIN MENU"<<"|"<<endl;
    cout<<setfill('_')<<setw(56)<<"|"<<"|"<<setfill(' ')<<endl;
    return get_num_inst();
}

/***************************************************************
 this function dispatch a package for the store
 its main feature is that it redirect the user to the issue form
 when the model which is meant to dispatch form the store is not
 found.
****************************************************************/
void export_pkg()
{
    char temp_model[10];
    cout<<"DISPATCH PACKAGE"<<endl;
    cout<<"here you can dispatch packages by their model"<<endl;
    cout<<"enter model :";
    cin.getline(temp_model,10);
    while(is_empty_string(temp_model)){
        cout<<"model can not be an empty string. please try again."<<endl;
        cin.getline(temp_model,10);
    }


    int result=search_by_model(temp_model,AVAILABLE);
    if(result==-1) //search was not successful
    {
        cout<<"WARRNING: the model you are trying to dispatch is not available in the current store"<<endl;
        cout<<"do you want to issue new property with this model("<<temp_model<<")?(y/n)"<<endl;
        if(yes_no())
            issue(temp_model);
    }
    else  //the search was successful
    {
        display(result);
        int quantity;
        cout<<"quantity: ";
        quantity=get_num('i');
        if(quantity>current_store[result].quantity)
        {
            cout<<"invalid quantity: the store does not have this much amount."<<endl;
            getche();
        }
        else
        {
            //adding to the dispatch package to dispatched status
            current_store[current_pkg_num]=current_store[result];
            current_store[current_pkg_num].quantity=quantity;
            current_store[current_pkg_num].stat=DISPATCHED;
            current_pkg_num++;
            //calculate the new quantity
            current_store[result].quantity-=quantity;

            if(write())
            {
                display(result);
                cout<<"successfully dispatched."<<endl;

                cout<<"do you went to dispatch more properties(y/n)"<<endl;
                if(yes_no())
                   export_pkg();
            }else{
                cout<<"ERROR: unable to save the change"<<endl;
                getch();
            }

        }
    }

}

/************************************************************************
this function will process i.e
                            1) will remove duplicate packages by adding
                            their quantity
                            2) will sort the packages by their status
                            3) will write (save) the processed
                            store by using write() function
************************************************************************/
void process()
{
    if(current_pkg_num>0)
    {
        int num=0;
        int b;

        for(int i=0; i<current_pkg_num; i++)
        {
            num++;
            b=num*100/current_pkg_num;
            system("cls");
            cout<<"processing "<<endl<<" ("<<b<<"%)"<<endl;
            for(int j=0; j<current_pkg_num; j++)
            {

                if(i==j)
                    continue;
                else if((strcmp(current_store[i].model,current_store[j].model)==0)&&(current_store[i].stat==current_store[j].stat))
                {
                    current_store[i].quantity+=current_store[j].quantity;

                    //delete the unwanted package record
                    current_pkg_num--;
                    for(int k=j; k<current_pkg_num; k++)
                    {
                        current_store[k]=current_store[k+1];
                    }
                }

            }
        }
        //sorting the packages
        pkg temp;
        for (int l=0;l<current_pkg_num;l++ )
        {
            for (int m=0;m<current_pkg_num;m++)
            {
                if((int)current_store[m].stat>(int)current_store[l].stat){
                    temp=current_store[m];
                    current_store[m]=current_store[l];
                    current_store[l]=temp;
                }

            }
        }


        system("cls");
        cout<<"done! the store is now nice and clear."<<endl;
        cout<<" (100%)"<<endl;
        write();//saving what was done
    }
    else
    {
        cout <<"the store is empty. can not process an empty store"<<endl;
    }
}

/****************************************************************
   this function will recored requested packages. it accepts a
    model(string) and search for other details in the current
    store and will ask for quantity if the search was a success.
    else it will ask for all other details
****************************************************************/
void issue(char model[]="NULL")
{


    pkg property;
    cout<<"	        issuing new property form"<<endl;
    cout<<"=============================================="<<endl;
    do
    {
        cout<<"model:";
        if(strcmp(model,"NULL")==0)
        {
            cin.getline(property.model,10);
            while(is_empty_string(property.model))
            {
                cout<<"model can not be an empty string. please try again."<<endl;
                cin.getline(property.model,10);
            }

        }
        else
        {
            strcpy(property.model,model);
            cout<<model<<endl;
        }
        int result=search_by_model(property.model);
        if(result==-1)
        {
            cout<<"serial no:";
            property.serial_no=get_num('i');
            cout<<"detail description :";
            cin.getline(property.description,50);
            while(is_empty_string(property.description))
            {
                cout<<"a description can not be an empty string. please try again."<<endl;
                cin.getline(property.description,10);
            }

            cout<<"quantity: ";
            property.quantity=get_num('i');
            cout<<"unit price: ";
            property.price=get_num();
        }
        else
        {
            cout<<"property found!"<<endl;
            display(result);
            property=current_store[result];
            cout<<"enter the amount you want to issue"<<endl;
            cout<<"quantity: ";
            property.quantity=get_num('i');
        }
        property.stat = ISSUED;
        current_store[current_pkg_num++]=property;
        process();
        cout<<"do you went to continue issuing?(y/n)"<<endl;
    }
    while(yes_no());

}

/*******************************************************
    this function will structure the flow of the
    program by integrating the other functions
*******************************************************/
int run()
{
    switch(main_menu())
    {
    case 0:
        exit(0);
        return 0;

    case 1:
        int store_menu_control;
        do
        {
            store_menu_control=store_menu();
            switch(store_menu_control)
            {
            case 0:
                break;

            case 1:
                system("cls");
                register_pkg();
                break;
            case 2:
                system("cls");
                display();
                break;
            case 3:
                system("cls");
                export_pkg();
                break;
            case 4:
                system("cls");
                issue();
                break;
            case 5:
                process();
                getche();
                break;
            case 6:
                int control;
                do
                {
                    control=report();
                    switch(control)
                    {
                    case 0:
                        break;
                    case 1:
                        display_report();
                        break;
                    case 2:
                        export_report();
                        break;
                    default:
                        system("cls");
                        cout<<"ERROR: the number you have inserted is invalid. please try again"<<endl;
                        getch();

                    }
                }
                while(control);
                break;
            case 7:
                export_file();
                break;
            default :
                system("cls");
                cout<<"ERROR: the number you have inserted is invalid. please try again"<<endl;
                getch();
            }
        }
        while(store_menu_control);
        return 1;
    case 2:
        help();
        return 1;
    case 3:
        about();
        return 1;
    case 4:
        issue();
        return 1;
    default :
        system("cls");
        cout<<"ERROR: the number you have inserted is invalid. please try again"<<endl;
        getch();
        run();
    }

}

/****************************************************************************
 this function will display the available file formats for exporting property list and
 exports on the directory that the program has been called to run. and it
 will the launch the exported file automatically.
****************************************************************************/

void export_file()
{
    system("cls");

    cout<<setfill('_')<<setw(56)<<" "<<endl<<left;
    cout<<setfill(' ')<<setw(56)<<"| choose the file type you went to export"<<"|"<<endl;
    cout<<setw(50)<<"|	1)HTML(web page)"<<"|"<<endl;
    cout<<setw(50)<<"|	2)CSV(spreed sheet)"<<"|"<<endl;
    cout<<setw(50)<<"|	3)TXT(text file)"<<"|"<<endl;
    cout<<setw(50)<<"|	0)back to REPORT MENU"<<"|"<<endl;
    cout<<setfill('_')<<setw(56)<<"|"<<"|"<<setfill(' ')<<endl;
    switch(get_num_inst())
    {
    case 0:
        break;
    case 1:
    {
        ofstream doc("html_file.html");
        if(doc.fail())
        {
            cout<<"ERROR: there was an error exporting the file"<<endl;
            getch();
        }
        else
        {
            cout<<"please wait..."<<endl;
            doc<<"<!DOCTYPE html><html><head><title>Departmental Store Management System</title><style>td{border: solid 2px #ddd;}tr:hover{background:#ddd;}</style></head><body><h2>property list</h2><table><tr>	<td>serial no.</td>	<td>model</td><td>quantity</td>	<td>unit price</td>	<td>status</td></tr> "<<endl;

            for(int i=0; i<current_pkg_num; i++)
                doc<<"<tr>"<<"<td>"<<current_store[i].serial_no<<"</td>"<<"<td>"<<current_store[i].model<<"</td>"<<"<td>"<<current_store[i].quantity<<"</td>"<<"<td>"<<current_store[i].price<<"</td>"<<"<td>"<<status_value[(int)current_store[i].stat]<<"</td>"<<"</tr>";
            doc<<"</table></body></html>";
            doc.close();
            system("start html_file.html");
        }
    }
    break;
    case 2:

    {
        ofstream csv("csv_file.csv");
        if(csv.fail())
        {
            cout<<"ERROR: there was an error exporting the file"<<endl;
            getch();
        }
        else
        {
            cout<<"please wait..."<<endl;
            csv<<"serial no., model ,description ,quantity ,unit price ,status "<<endl;
            for(int i=0; i<current_pkg_num; i++)
                csv<<current_store[i].serial_no<<","<<current_store[i].model<<","<<current_store[i].description<<","<<current_store[i].quantity<<","<<current_store[i].price<<","<<status_value[(int)current_store[i].stat]<<","<<endl;
            csv.close();
            system("start csv_file.csv");
        }
    }
    break;
    case 3:
    {
        ofstream doc("txt_file.txt");
        if(doc.fail())
        {
            cout<<"ERROR: there was an error exporting the file"<<endl;
            getch();
        }
        else
        {
            cout<<"please wait..."<<endl;
            doc<<" "<<setfill('_')<<setw(98)<<"_"<<" "<<setfill(' ')<<endl;
            doc<<"|"<<left<<setw(9)<<"serial no. "<<setw(10)<<"|model"<<setw(25)<<"|description"<<setw(10)<<"|quantity"<<"|"<<setw(20)<<right<<"unit price "<<"|"<<setw(20)<<left<<"status "<<"|"<<endl;
            doc<<setfill('=')<<setw(12)<<"|"<<setw(10)<<"|"<<setw(25)<<"|"<<setw(10)<<"|"<<setw(21)<<"|"<<setw(21)<<"|"<<"|"<<setfill(' ')<<endl;

            for(int i=0; i<current_pkg_num; i++)
                doc<<left<<"|"<<setw(11)<<current_store[i].serial_no<<"|"<<setw(9)<<current_store[i].model<<"|"<<setw(24)<<current_store[i].description<<"|"<<setw(9)<<current_store[i].quantity<<"|"<<setw(20)<<right<<current_store[i].price<<"|"<<setw(20)<<right<<status_value[(int)current_store[i].stat]<<"|"<<endl;
            doc<<"|"<<setfill('_')<<setw(12)<<"|"<<setw(10)<<"|"<<setw(25)<<"|"<<setw(10)<<"|"<<setw(21)<<"|"<<setw(21)<<"|"<<setfill(' ')<<endl;
            doc.close();
            system("start txt_file.txt");
        }
    }
    break;

    default :
        system("cls");
        cout<<"ERROR: the number you have inserted is invalid. please try again"<<endl;
        getch();
    }
}

/****************************************************************************
 this function will display the available file formats for exporting report and
 exports on the directory that the program has been called to run. and it
 will the launch the exported file automatically.
****************************************************************************/

void export_report()
{
    system("cls");

    cout<<setfill('_')<<setw(56)<<" "<<endl<<left;
    cout<<setfill(' ')<<setw(56)<<"| choose the file type you went to export"<<"|"<<endl;
    cout<<setw(50)<<"|	1)HTML(web page)"<<"|"<<endl;
    cout<<setw(50)<<"|	2)CSV(spreed sheet)"<<"|"<<endl;
    cout<<setw(50)<<"|	3)TXT(text file)"<<"|"<<endl;
    cout<<setw(50)<<"|	4)DOC(word document)"<<"|"<<endl;
    cout<<setw(50)<<"|	0)back to REPORT MENU"<<"|"<<endl;
    cout<<setfill('_')<<setw(56)<<"|"<<"|"<<setfill(' ')<<endl;

    long double total_asset=0;
    long double total_dispatch=0;
    long double total_issued=0;

    //calculating the total
    for(int i=0; i<current_pkg_num; i++)
    {
        switch(current_store[i].stat)
        {
        case AVAILABLE:
            total_asset+=(current_store[i].price*current_store[i].quantity);
            break;
        case ISSUED:
            total_issued+=(current_store[i].price*current_store[i].quantity);
            break;
        case DISPATCHED:
            total_dispatch+=(current_store[i].price*current_store[i].quantity);
            break;
        }
    }


    switch(get_num_inst())
    {
    case 0:
        break;
    case 1:
    {
        ofstream doc("html_report_file.html");
        if(doc.fail())
        {
            cout<<"ERROR: there was an error exporting the file"<<endl;
            getch();
        }
        else
        {
            cout<<"please wait..."<<endl;
            doc<<"<!DOCTYPE HTML><HTML><head><title>store report</title><style>td{border:solid #ddd;}tr:hover{background:#ddd;}span{color:green;}</style></head><body><h2>store report</h2><p>the following table shows property model with its corresponding percentage of its price compared with the total asset of the store.</p><table><tr><td>model</td><td>percentage(%)</td></tr>"<<endl;

            for(int j=0; j<current_pkg_num; j++)
            {
                if(current_store[j].stat!=AVAILABLE)
                    continue;
                doc<<"<tr><td>"<<current_store[j].model<<"</td><td>"<<(current_store[j].price*current_store[j].quantity)*100/total_asset<<"</td></tr>"<<endl;
            }

            doc<<"</table><p><h3>summary</h3>current store has total asset of <span>"<<total_asset<<"</span> birr</br>issued properties will cost <span>"<<total_issued<<"</span> birr</br>dispatched properties cost <span>"<< total_dispatch <<"</span> birr</br></p>"<<endl;
            doc.close();
            system("start html_report_file.html");
        }
    }
    break;
    case 2:

    {
        ofstream csv("csv_report_file.csv");
        if(csv.fail())
        {
            cout<<"ERROR: there was an error exporting the file"<<endl;
            getch();
        }
        else
        {
            cout<<"please wait..."<<endl;
            csv<<"the following table shows property model with its corresponding percentage of its price compared with the total asset of the store."<<endl;
            csv<<"model ,percentage(%)"<<endl;

            for(int j=0; j<current_pkg_num; j++)
            {
                if(current_store[j].stat!=AVAILABLE)
                    continue;
                csv<<current_store[j].model<<","<<(current_store[j].price*current_store[j].quantity)*100/total_asset<<endl;
            }
            csv<<"current store has total asset of,"<<total_asset<<",birr"<<endl;
            csv<<"issued properties will cost,"<<total_issued<<",birr"<<endl;
            csv<<"dispatched properties cost,"<<total_dispatch<<",birr"<<endl;

            csv.close();
            system("start csv_report_file.csv");
        }
    }
    break;
    case 3:
    {
        ofstream doc("doc_report_file.txt");
        if(doc.fail())
        {
            cout<<"ERROR: there was an error exporting the file"<<endl;
            getch();
        }
        else
        {
            cout<<"please wait..."<<endl;
            doc<<setfill('_')<<setw(56)<<" "<<endl<<left;
            doc<<setfill(' ')<<setw(56)<<"| REPORT"<<"|"<<endl;
            doc<<setw(56)<<"| the following table shows property model with its "<<"|"<<endl;
            doc<<setw(56)<<"| corresponding percentage of its price compared with "<<"|"<<endl;
            doc<<setw(56)<<"| the total asset of the store."<<"|"<<endl;
            doc<<"|"<<setfill('-')<<"+"<<setw(23)<<"-"<<"+"<<setfill(' ')<<setw(30)<<" "<<"|"<<endl;
            doc<<setw(11)<<"||model"<<setw(14)<<"|percentage(%)"<<"|"<<setw(30)<<" "<<"|"<<endl;
            doc<<"|"<<setfill('-')<<"+"<<setw(23)<<"-"<<"+"<<setfill(' ')<<setw(30)<<" "<<"|"<<endl;

            for(int j=0; j<current_pkg_num; j++)
            {
                if(current_store[j].stat!=AVAILABLE)
                    continue;
                doc<<"||"<<setw(9)<<current_store[j].model<<"|"<<setw(13)<<right<<(current_store[j].price*current_store[j].quantity)*100/total_asset<<left<<"|"<<setw(30)<<" "<<"|"<<endl;
            }
            doc<<"|"<<setfill('-')<<"+"<<setw(23)<<"-"<<"+"<<setfill(' ')<<setw(30)<<" "<<"|"<<endl;
            doc<<setfill('_')<<setw(56)<<"|"<<"|"<<setfill(' ')<<endl;


            doc<<"|"<<"current store has total asset of "<<total_asset<<" birr"<<endl;
            doc<<"|"<<"issued properties will cost "<<total_issued<<" birr"<<endl;
            doc<<"|"<<"dispatched properties cost "<<total_dispatch<<" birr"<<endl;
            doc<<setfill('_')<<setw(56)<<"|"<<"|"<<setfill(' ')<<endl;

            doc.close();
            system("start doc_report_file.txt");
        }
    }
    break;
    case 4:
    {
        ofstream doc("doc_report_file.doc");
        if(doc.fail())
        {
            cout<<"ERROR: there was an error exporting the file"<<endl;
            getch();
        }
        else
        {
            cout<<"please wait..."<<endl;
            doc<<setfill(' ')<<setw(56)<<left<<" REPORT"<<endl;
            doc<<"the following table shows property model with its corresponding percentage of its price compared with the total asset of the store."<<endl;
            doc<<setfill('-')<<"+"<<setw(23)<<"-"<<"+"<<setfill(' ')<<endl;
            doc<<setw(10)<<"|model"<<setw(14)<<"|percentage(%)"<<"|"<<endl;
            doc<<setfill('-')<<"+"<<setw(23)<<"-"<<"+"<<setfill(' ')<<endl;

            for(int j=0; j<current_pkg_num; j++)
            {
                if(current_store[j].stat!=AVAILABLE)
                    continue;
                doc<<"|"<<setw(9)<<current_store[j].model<<"|"<<setw(13)<<right<<(current_store[j].price*current_store[j].quantity)*100/total_asset<<left<<"|"<<endl;
            }
            doc<<setfill('-')<<"+"<<setw(23)<<"-"<<"+"<<setfill(' ')<<endl;
            doc<<"current store has total asset of "<<total_asset<<" birr"<<endl;
            doc<<"issued properties will cost "<<total_issued<<" birr"<<endl;
            doc<<"dispatched properties cost "<<total_dispatch<<" birr"<<endl;

            doc.close();
            system("start doc_report_file.doc");
        }
    }
    break;

    default :
        system("cls");
        cout<<"ERROR: the number you have inserted is invalid. please try again"<<endl;
        getch();
    }
}

/********************************************************
    this function displays report menu
*********************************************************/
int report()
{
    system("cls");

    cout<<setfill('_')<<setw(56)<<" "<<endl<<left;
    cout<<setfill(' ')<<setw(56)<<"| REPORT MENU"<<"|"<<endl;
    cout<<setw(50)<<"|	1)display report"<<"|"<<endl;
    cout<<setw(50)<<"|	2)export report to other file formats"<<"|"<<endl;
    cout<<setw(50)<<"|	0)back to STORE MENU"<<"|"<<endl;
    cout<<setfill('_')<<setw(56)<<"|"<<"|"<<setfill(' ')<<endl;
    return get_num_inst();
}
/********************************************************
    this function displays report of the current store
*********************************************************/
void display_report()
{

    long double total_asset=0;
    long double total_dispatch=0;
    long double total_issued=0;

    //calculating the total
    for(int i=0; i<current_pkg_num; i++)
    {
        switch(current_store[i].stat)
        {
        case AVAILABLE:
            total_asset+=(current_store[i].price*current_store[i].quantity);
            break;
        case ISSUED:
            total_issued+=(current_store[i].price*current_store[i].quantity);
            break;
        case DISPATCHED:
            total_dispatch+=(current_store[i].price*current_store[i].quantity);
            break;
        }
    }
    cout<<setfill('_')<<setw(56)<<" "<<endl<<left;
    cout<<setfill(' ')<<setw(56)<<"| DESPLAYING THE REPORT"<<"|"<<endl;
    cout<<setw(56)<<"| the following table shows property model with its "<<"|"<<endl;
    cout<<setw(56)<<"| corresponding percentage of its price compared with "<<"|"<<endl;
    cout<<setw(56)<<"| the total asset of the store. note that the percentage"<<"|"<<endl;
    cout<<setw(56)<<"| is rounded."<<"|"<<endl;
    cout<<"|"<<setfill('-')<<"+"<<setw(23)<<"-"<<"+"<<setfill(' ')<<setw(30)<<" "<<"|"<<endl;
    cout<<setw(11)<<"||model"<<setw(14)<<"|percentage(%)"<<"|"<<setw(30)<<" "<<"|"<<endl;
    cout<<"|"<<setfill('-')<<"+"<<setw(23)<<"-"<<"+"<<setfill(' ')<<setw(30)<<" "<<"|"<<endl;

    for(int j=0; j<current_pkg_num; j++)
    {
        if(current_store[j].stat!=AVAILABLE)
            continue;
        cout<<"||"<<setw(9)<<current_store[j].model<<"|"<<setw(13)<<right<<(current_store[j].price*current_store[j].quantity)*100/total_asset<<left<<"|"<<setw(30)<<" "<<"|"<<endl;
    }
    cout<<"|"<<setfill('-')<<"+"<<setw(23)<<"-"<<"+"<<setfill(' ')<<setw(30)<<" "<<"|"<<endl;
    cout<<setfill('_')<<setw(56)<<"|"<<"|"<<setfill(' ')<<endl;


    cout<<"|"<<"current store has total asset of "<<total_asset<<" birr"<<endl;
    cout<<"|"<<"issued properties will cost "<<total_issued<<" birr"<<endl;
    cout<<"|"<<"dispatched properties cost "<<total_dispatch<<" birr"<<endl;
    cout<<setfill('_')<<setw(56)<<"|"<<"|"<<setfill(' ')<<endl;


    getch();

}

// this function shows the main menu
int main_menu()
{
    system("cls");

    cout<<setfill('_')<<setw(56)<<" "<<endl<<left;
    cout<<setfill(' ')<<setw(56)<<"| MAIN MENU"<<"|"<<endl;
    cout<<setw(50)<<"|	1)store menu"<<"|"<<endl;
    cout<<setw(50)<<"|	2)help"<<"|"<<endl;
    cout<<setw(50)<<"|	3)about"<<"|"<<endl;
    cout<<setw(50)<<"|	0)exit"<<"|"<<endl;
    cout<<setfill('_')<<setw(56)<<"|"<<"|"<<setfill(' ')<<endl;
    return get_num_inst();
}
// this function displays the information about the developers and the program
void about()
{
    system("cls");

    cout<<setfill('_')<<setw(56)<<" "<<endl<<left;
    cout<<setfill(' ')<<setw(56)<<"|    ABOUT"<<"|"<<endl;
    cout<<setw(56)<<"|"<<"|"<<endl;
    cout<<setw(50)<<"|	Name :Departmental Store Management System"<<"|"<<endl;
    cout<<setw(50)<<"|	Version: 0.01"<<"|"<<endl;
    cout<<setw(50)<<"|	Developers:REDET GETACHEW(3928/08)"<<"|"<<endl;
    cout<<setw(50)<<"|	           ZELALEM ZERFU(4046/08)"<<"|"<<endl;
    cout<<setw(50)<<"|	           TAMIRESILASSIE TILAHUN(3974/08)"<<"|"<<endl;
    cout<<setw(50)<<"|	           YOHANNES TADESSE(4036/08)"<<"|"<<endl;
    cout<<setw(50)<<"|	           YOHANNES NIGUSSIE(4033/08)"<<"|"<<endl;
    cout<<setw(50)<<"|	 "<<"|"<<endl;
    cout<<setw(50)<<"|	(c) Copyright 2016. All Rights Reserved."<<"|"<<endl;
    cout<<setfill('_')<<setw(56)<<"|"<<"|"<<setfill(' ')<<endl;
    getch();

}

void greet()
{
    system("cls");
cout<<" _    _      _                           "<<endl;
cout<<"| |  | | ___| | ___ ___  _ __ ___   ___  "<<endl;
cout<<"| |/\\| |/ _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\ "<<endl;
cout<<"\\  /\\  /  __/ | (_| (_) | | | | | |  __/ "<<endl;
cout<<" \\/  \\/ \\___|_|\\___\\___/|_| |_| |_|\\___| "<<endl;
cout<<" ____                        _                        _        _   ____  _                 "<<endl;
cout<<"|  _ \\  ___ _ __   __ _ _ __| |_ _ __ ___   ___ _ __ | |_ __ _| | / ___|| |_ ___  _ __ ___ "<<endl;
cout<<"| | | |/ _ \\ '_ \\ / _` | '__| __| '_ ` _ \\ / _ \\ '_ \\| __/ _` | | \\___ \\| __/ _ \\| '__/ _ \\"<<endl;
cout<<"| |_| |  __/ |_) | (_| | |  | |_| | | | | |  __/ | | | || (_| | |  ___) | || (_) | | |  __/"<<endl;
cout<<"|____/ \\___| .__/ \\__,_|_|   \\__|_| |_| |_|\\___|_| |_|\\__\\__,_|_| |____/ \\__\\___/|_|  \\___|"<<endl;
cout<<"           |_|                                                                             "<<endl;
cout<<" __  __                                                   _     ____            _           "<<endl;
cout<<"|  \\/  | __ _ _ __   __ _  __ _  ___ _ __ ___   ___ _ __ | |_  / ___| _   _ ___| |_ ___ _ __ ___  "<<endl;
cout<<"| |\\/| |/ _` | '_ \\ / _` |/ _` |/ _ \\ '_ ` _ \\ / _ \\ '_ \\| __| \\___ \\| | | / __| __/ _ \\ '_ ` _ \\ "<<endl;
cout<<"| |  | | (_| | | | | (_| | (_| |  __/ | | | | |  __/ | | | |_   ___) | |_| \\__ \\ ||  __/ | | | | |"<<endl;
cout<<"|_|  |_|\\__,_|_| |_|\\__,_|\\__, |\\___|_| |_| |_|\\___|_| |_|\\__| |____/ \\__, |___/\\__\\___|_| |_| |_|"<<endl;
cout<<"                           |___/                                       |___/                       "<<endl;
cout<<"press any key to continue ..."<<endl;
getch();
}

void help(){
    system("cls");
    cout<<setfill('_')<<setw(60)<<" "<<endl<<left<<setfill(' ');
    cout<<setw(60)<<"|WHAT IS DEPARTMENTAL STORE MANAGEMENT SYSTEM?"<<"|"<<endl;
    cout<<setfill('=')<<setw(60)<<"|"<<"|"<<setfill(' ')<<endl;
    cout<<setw(60)<<"|"<<"|"<<endl;
    cout<<setw(60)<<"|Departmental store management system is a program "<<"|"<<endl;
    cout<<setw(60)<<"|that helps to manage the store of a certain orga-"<<"|"<<endl;
    cout<<setw(60)<<"|nization, for example in university of Gondar "<<"|"<<endl;
    cout<<setw(60)<<"|there is a big store that other departments use "<<"|"<<endl;
    cout<<setw(60)<<"|and depend on in order to function properly. This"<<"|"<<endl;
    cout<<setw(60)<<"|program is intended to facilitate the function of "<<"|"<<endl;
    cout<<setw(60)<<"|such unit of an organization by providing the "<<"|"<<endl;
    cout<<setw(60)<<"|following functions."<<"|"<<endl;
    cout<<setw(60)<<"|"<<"|"<<endl;
    cout<<setw(60)<<"|-Managing recodes of properties that the store has:-"<<"|"<<endl;
    cout<<setw(60)<<"| this has a certain advantage over the paper paced "<<"|"<<endl;
    cout<<setw(60)<<"| system. By using this program store manager or any "<<"|"<<endl;
    cout<<setw(60)<<"| other employee can instantly access the available "<<"|"<<endl;
    cout<<setw(60)<<"| products that the store contain. By doing so not "<<"|"<<endl;
    cout<<setw(60)<<"| only this system can save time but also money that"<<"|"<<endl;
    cout<<setw(60)<<"| was wasted in the older system."<<"|"<<endl;
    cout<<setw(60)<<"|"<<"|"<<endl;
    cout<<setw(60)<<"|-Managing the issued properties that the store doesn't"<<"|"<<endl;
    cout<<setw(60)<<"| have:- this is important in a way that the issued pr-"<<"|"<<endl;
    cout<<setw(60)<<"| oducts can be managed easly."<<"|"<<endl;
    cout<<setw(60)<<"|"<<"|"<<endl;
    cout<<setw(60)<<"|-Recoding dispatched properties:- dispatched properties"<<"|"<<endl;
    cout<<setw(60)<<"| are products that have been sent to other departments."<<"|"<<endl;
    cout<<setw(60)<<"| this way the store manager knows how it has performed"<<"|"<<endl;
    cout<<setw(60)<<"| and how much money the store have served."<<"|"<<endl;
    cout<<setw(60)<<"|"<<"|"<<endl;
    cout<<setw(60)<<"|-Producing and exporting reports to other file formats:-"<<"|"<<endl;
    cout<<setw(60)<<"| one of the main feature of this program is its ability"<<"|"<<endl;
    cout<<setw(60)<<"| to produce reports. This way one can easily understand"<<"|"<<endl;
    cout<<setw(60)<<"| the performance of the store. And this reports can "<<"|"<<endl;
    cout<<setw(60)<<"| easily exported to other file formats, like word docu-"<<"|"<<endl;
    cout<<setw(60)<<"| ments and spread sheet, for further analysis."<<"|"<<endl;
    cout<<setw(60)<<"|"<<"|"<<endl;
    cout<<setw(60)<<"|HERE YOU CAN FIND OUT HOW YOU CAN USE THE PROGRAM."<<"|"<<endl;
    cout<<setfill('=')<<setw(60)<<"|"<<"|"<<setfill(' ')<<endl;
    cout<<setw(60)<<"|"<<"|"<<endl;
    cout<<setw(60)<<"|The program  have 3 menus those are main menu, "<<"|"<<endl;
    cout<<setw(60)<<"|store menu and report menu. We will in detail "<<"|"<<endl;
    cout<<setw(60)<<"|each menus."<<"|"<<endl;
    cout<<setw(60)<<"|"<<"|"<<endl;
    cout<<setw(60)<<"|MAIN MENU:-main menu is the first menu you see"<<"|"<<endl;
    cout<<setw(60)<<"| after the program is lunched. In this menu there"<<"|"<<endl;
    cout<<setw(60)<<"| are 4 options those are 1)store menu  this option"<<"|"<<endl;
    cout<<setw(60)<<"| leads the store menu in which you can do store "<<"|"<<endl;
    cout<<setw(60)<<"| operations. this menu will be discussed in great "<<"|"<<endl;
    cout<<setw(60)<<"| detail. The 2nd option is help which display this "<<"|"<<endl;
    cout<<setw(60)<<"| section. Next option is about. this section contains"<<"|"<<endl;
    cout<<setw(60)<<"| information about the version of the program and "<<"|"<<endl;
    cout<<setw(60)<<"| contains names developers plus copy right information."<<"|"<<endl;
    cout<<setw(60)<<"|"<<"|"<<endl;
    cout<<setw(60)<<"|STORE MENU:- this menu contains the main operation"<<"|"<<endl;
    cout<<setw(60)<<"| of the store those are 1) register/import property"<<"|"<<endl;
    cout<<setw(60)<<"| option. This option allows the you to register the"<<"|"<<endl;
    cout<<setw(60)<<"| properties the store currently has. This option is"<<"|"<<endl;
    cout<<setw(60)<<"| made easy to make the registration easy by automat-"<<"|"<<endl;
    cout<<setw(60)<<"| ically reaching for identical property and filling"<<"|"<<endl;
    cout<<setw(60)<<"| the most of the information that would otherwise "<<"|"<<endl;
    cout<<setw(60)<<"| consume time and make the data encoder bored. Note"<<"|"<<endl;
    cout<<setw(60)<<"| that if you try to register the issued property it"<<"|"<<endl;
    cout<<setw(60)<<"| informs you and it will hand the reset of the ope-"<<"|"<<endl;
    cout<<setw(60)<<"| ration.  The next option is option number 2 which"<<"|"<<endl;
    cout<<setw(60)<<"| is “display property” as the name indicates it "<<"|"<<endl;
    cout<<setw(60)<<"| display the entire store records in tabular form."<<"|"<<endl;
    cout<<setw(60)<<"| The 3rd option is named dispatch property the name"<<"|"<<endl;
    cout<<setw(60)<<"| is self explanatory. It does what it name tails. "<<"|"<<endl;
    cout<<setw(60)<<"| You dispatching a property when other unit of you"<<"|"<<endl;
    cout<<setw(60)<<"| organization export that property from the store "<<"|"<<endl;
    cout<<setw(60)<<"| department. The 4th option is issue property this"<<"|"<<endl;
    cout<<setw(60)<<"| option display the issuing  form and again to make"<<"|"<<endl;
    cout<<setw(60)<<"| data encoding easier is will search for identical "<<"|"<<endl;
    cout<<setw(60)<<"| property and fill all necessary information autom-"<<"|"<<endl;
    cout<<setw(60)<<"| atically. The 5th option is named “process and sort"<<"|"<<endl;
    cout<<setw(60)<<"| the store” this option will search for duplicated "<<"|"<<endl;
    cout<<setw(60)<<"| or separated records and concatenate those records"<<"|"<<endl;
    cout<<setw(60)<<"| in such a way that it preserves all the information"<<"|"<<endl;
    cout<<setw(60)<<"| making the store more efficient. Plus this option"<<"|"<<endl;
    cout<<setw(60)<<"| sorts the record by their status. The next option"<<"|"<<endl;
    cout<<setw(60)<<"| is report menu which it located at number 6 and it"<<"|"<<endl;
    cout<<setw(60)<<"| will be discussed in the next paragraph for now it"<<"|"<<endl;
    cout<<setw(60)<<"| navigates you to report menu. Last but not the least"<<"|"<<endl;
    cout<<setw(60)<<"| option is number 7)export property list to other file"<<"|"<<endl;
    cout<<setw(60)<<"| formats this option allows you to export the entire "<<"|"<<endl;
    cout<<setw(60)<<"| list to html, csv and txt this way you can farther "<<"|"<<endl;
    cout<<setw(60)<<"| do analysis. There is also another option for expo-"<<"|"<<endl;
    cout<<setw(60)<<"| rting reports in other file formats in the report "<<"|"<<endl;
    cout<<setw(60)<<"| menu."<<"|"<<endl;
    cout<<setw(60)<<"|"<<"|"<<endl;
    cout<<setw(60)<<"|Report menu:- this menu is all about reports. the "<<"|"<<endl;
    cout<<setw(60)<<"| first option shows summarized information of the "<<"|"<<endl;
    cout<<setw(60)<<"| store both in tabular form and textual form. The "<<"|"<<endl;
    cout<<setw(60)<<"| 2nd option is for exporting  the report in to other"<<"|"<<endl;
    cout<<setw(60)<<"| file formats for example word document , excel "<<"|"<<endl;
    cout<<setw(60)<<"| sheets e.t.c."<<"|"<<endl;
    cout<<setw(60)<<"|"<<"|"<<endl;
    cout<<setw(60)<<"|Option number zero(0) is a special option for naviga-"<<"|"<<endl;
    cout<<setw(60)<<"|ting back to root menu and in main menu for exiting "<<"|"<<endl;
    cout<<setw(60)<<"|the program."<<"|"<<endl;
    cout<<setw(60)<<"|"<<"|"<<endl;
    cout<<setw(60)<<"|NOTE: you can press the number labeled to those option"<<"|"<<endl;
    cout<<setw(60)<<"| to select them."<<"|"<<endl;
    cout<<setw(60)<<"|"<<"|"<<endl;
    cout<<setw(60)<<"|WARRING: please do not delete the file called bin_db.dat"<<"|"<<endl;
    cout<<setw(60)<<"| or else entire data or the store managing program will "<<"|"<<endl;
    cout<<setw(60)<<"| be lost."<<"|"<<endl;
    cout<<setfill('_')<<setw(60)<<"|"<<"|"<<setfill(' ')<<endl;
    getch();
}
