#include <stdio.h>
#include <mbedtls/bignum.h>

/* simple program for large integer multiplication */

int main()
{

    mbedtls_mpi a;
    mbedtls_mpi b;
    mbedtls_mpi c;

#ifdef DEBUG
    mbedtls_mpi ans;
    mbedtls_mpi_init(&ans);
    mbedtls_mpi_grow(&ans, 64);
    mbedtls_mpi_read_string(&ans, 10, "982596486168164533679068138587450785169755353624059289415533430269902156543301980948452346231423154225707198230814993348325199962226568104628565333075845055110102001513777733822186085910101000971046266247429174698114691926560611342709382763139596083197537103114770536908672391590105944592251557906143423196875007652523485788407858534962495117364862292894053758230284655296335742210979223770143291569366072997986170990930100185641047797304056518425097161421304246083157927306637095564491267011456237384260952278839451893481896247389483799171694323752613825934682293057818579843667187625791606764793287485398270601000");
#endif

    mbedtls_mpi_init(&a);
    mbedtls_mpi_init(&b);
    mbedtls_mpi_init(&c);

    mbedtls_mpi_grow(&a, 32); /* Make a hold a 1024-bit integer */
    mbedtls_mpi_grow(&b, 32); /* Make b hold a 1024-bit integer */
    mbedtls_mpi_grow(&c, 64); /* Make c hold a 2048-bit integer */

    mbedtls_mpi_read_string(&a, 10, "105063345230616104657348013634297701786006542128941909083101185888832813981927541578449103885990606406091215423988641232290414368860661235234905516208831410002395649542617648887800946357680235133745987328106517234870888931439344639465469355722657253870347038180634028206997149618094761699250765049161158009075");
    mbedtls_mpi_read_string(&b, 10, "9352419571367596976939279289180413447699593351180217187280653361389412531812702891792282763505995136608679026507582290903916195465065737145139940492408335268162359576996486883373274104291965294741841070659192427935117575863981847460287191342833560356018485176253542816589237713501814110266911435430483265080");
    mbedtls_mpi_read_string(&c, 10, "0"); 

    int ret = mbedtls_mpi_mul_mpi(&c, &a, &b);
    if(ret != 0) {
        return -1;
    }

#ifdef DEBUG
    ret = mbedtls_mpi_cmp_mpi(&c, &ans);
    if(ret != 0) {
        printf("Did not get expected answer\n");
        return -1;
    }
    else {
        printf("Answer is correct\n");
        return -1;
    }
    mbedtls_mpi_free(&ans);
#endif

    mbedtls_mpi_free(&a);
    mbedtls_mpi_free(&b);
    mbedtls_mpi_free(&c);

    return 0;
}
