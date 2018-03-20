#include<stdio.h>
#include<string.h>
#include<stdlib.h>
int main(int argc, const char* argv[])
{
    FILE *input;
    char buffer[100];
    char gv2 = argv[2][0];

    input = fopen(argv[1], "rb");
    if(input == NULL)
    {
        printf("open failure");
    }
    else
    {
        printf("----------Input file example.txt----------\n");
        while(fgets(buffer, 100, input) != NULL)
        {
            int addr = 0;
            char decide[100];
            char reverse[100];
            char split[100];
            //printf("Before decide is: %s\n", decide);
            //printf("Before buffer is: %s", buffer);
            while(buffer[addr] != ' ' && addr < strlen(buffer))
            {
                decide[addr] = buffer[addr];
                addr++;
            } 
            addr++;
            for(int i = 0; i < strlen(decide); i++)
                if(decide[i] == ' ')
                    decide[i] = '\0';
            //printf("addr is: %d\n", addr);
            //printf("Decide is: %s\n", decide);
            //printf("Decide size is: %d\n", strlen(decide));
            //printf("Buffer size is: %d\n", strlen(buffer));
            printf("%s", buffer);
            if(strcmp(decide, "reverse") == 0)
            {
                int rev_addr = 0;
                for(int i=strlen(buffer)-2; i>=addr; i--)
                {
                    reverse[rev_addr] = buffer[i];
                    rev_addr++;
                }
                for(int i = 0; i < strlen(reverse); i++)
                    if(i == rev_addr)
                        reverse[i] = '\0';
                printf("%s\n", reverse);
            }
            else if(strcmp(decide, "split") == 0)
            {
                int spl_addr = 0;
                for(int i=addr; i<=strlen(buffer)-2; i++)
                {
                    split[spl_addr] = buffer[i];
                    spl_addr++;
                }
                for(int i = 0; i < strlen(split); i++)
                    if(i == spl_addr)
                        split[i] = '\0';
                int be = 0, af = 0, state = 0, addr_gv2 = 0;
                //printf("Length of argv[2]: %d\n", strlen(argv[2]));
                while(be < strlen(split))
                {
                    if(argv[2][addr_gv2] == split[be])
                    {
                        addr_gv2++;
                    }
                    else
                    {
                        if(addr_gv2 != 0)
                        {
                            while(addr_gv2 != 0)
                            {
                                split[af] = split[be - addr_gv2];
                                addr_gv2--;
                                af++;
                            }
                            if(split[be] == argv[2][0])
                                addr_gv2 = 1;
                            else
                            {
                                split[af] = split[be];
                                af++;
                            }
                        }
                        else
                        {
                            split[af] = split[be];
                            af++;
                        }
                    }
                    be++;
                }
                while(addr_gv2 != 0)
                {
                    split[af] = split[be - addr_gv2];
                    addr_gv2--;
                    af++;
                }
                for(int i = 0; i < strlen(split); i++)
                    if(i == af)
                        split[i] = '\0';
                printf("%s\n", split);
            }
            else if(strcmp(decide, "exit") == 0)
                exit(0);
            else
                printf("YOUR INPUT IS NOT CORRECT.\nKEY WORDS ARE 'reverse' and 'split'.\n");
            memset(decide, ' ', 50);
            memset(reverse, ' ', 50);
            memset(split, ' ', 50);
        }
        printf("----------End of input file example.txt----------\n");
    }
    printf("**********User input**********\n");
    
        char _buffer[100];
        char _decide[100];
        char _reverse[100];
        char _split[100];
        while(gets(_buffer))
        {
            int _addr = 0;
            while(_buffer[_addr] != ' ' && _addr < strlen(_buffer))
            {
                _decide[_addr] = _buffer[_addr];
                _addr++;
            } 
            _addr++;
            //printf("_addr is: %d\n", _addr);
            //printf("_decide is: %s\n", _decide);
            //printf("Buffer size is: %d\n", strlen(buffer));
            for(int i = 0; i < strlen(_decide); i++)
                if(_decide[i] == ' ')
                    _decide[i] = '\0';
            //printf("Decide is: %s\n", _decide);
            //printf("Before: %s\n", _buffer);
            if(strcmp(_decide, "reverse") == 0)
            {
                int _rev_addr = 0;
                for(int i=strlen(_buffer)-1; i>=_addr; i--)
                {
                    _reverse[_rev_addr] = _buffer[i];
                    _rev_addr++;
                }
                for(int i = 0; i < strlen(_reverse); i++)
                    if(i == _rev_addr)
                        _reverse[i] = '\0';
                printf("%s\n", _reverse);
            }
            else if(strcmp(_decide, "split") == 0)
            {
                int _spl_addr = 0;
                for(int i=_addr; i<=strlen(_buffer)-1; i++)
                {
                    _split[_spl_addr] = _buffer[i];
                    _spl_addr++;
                }
                for(int i = 0; i < strlen(_split); i++)
                    if(i == _spl_addr)
                        _split[i] = '\0';
                int _be = 0, _af = 0, _state = 0, _addr_gv2 = 0;
                while(_be < strlen(_split))
                {
                    if(argv[2][_addr_gv2] == _split[_be])
                    {
                        _addr_gv2++;
                    }
                    else
                    {
                        if(_addr_gv2 != 0)
                        {
                            while(_addr_gv2 != 0)
                            {
                                _split[_af] = _split[_be - _addr_gv2];
                                _addr_gv2--;
                                _af++;
                            }
                            if(_split[_be] == argv[2][0])
                                _addr_gv2 = 1;
                            else
                            {
                                _split[_af] = _split[_be];
                                _af++;
                            }
                        }
                        else
                        {
                            _split[_af] = _split[_be];
                            //printf("%c\n", _splitp[_af]);
                            _af++;
                        }
                    }
                    if(_addr_gv2 == strlen(argv[2]))
                    {
                        _split[_af] = ' ';
                        _addr_gv2 = 0;
                        _af++;
                    }
                    _be++;
                }
                while(_addr_gv2 != 0)
                {
                    _split[_af] = _split[_be - _addr_gv2];
                    _addr_gv2--;
                    _af++;
                }
                for(int i = 0; i < strlen(_split); i++)
                    if(i == _af)
                        _split[i] = '\0';
                printf("%s\n", _split);
            }
            else if(strcmp(_decide, "exit") == 0)
                exit(0);
            else
                printf("YOUR INPUT IS NOT CORRECT.\nKEY WORDS ARE 'reverse' and 'split'.\n");
            memset(_decide, ' ', 50);
            memset(_reverse, ' ', 50);
            memset(_split, ' ', 50); 
        }
        return 0;
}
