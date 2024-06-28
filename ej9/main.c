


int main(int argc, char** argv){
    FILE* fp;
    if(argc < 2 ) {
        return 1;
    }
    fp = fopen("w",argv[1]);
    fclose(fp);

    return 0;
}