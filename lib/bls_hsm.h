#ifndef bls_hsm_h
#define bls_hsm_h
#include "blst.h"

#define MAX_KEYSTORE_SIZE 10

blst_scalar sk;
blst_scalar secret_keys_store[MAX_KEYSTORE_SIZE];
blst_scalar sk_sign;
char public_keys_hex_store[MAX_KEYSTORE_SIZE][96];
int keystore_size = 0;

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
int get_keystore_size(){
        return keystore_size;
}

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
void store_pk(char* public_key_hex){
        int cont = keystore_size - 1;
        for(int i = 0; i < 96; i++){
            public_keys_hex_store[cont][i] = public_key_hex[i];
        }
}

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
void getkeys(char public_keys_hex_store_ns[keystore_size][96]){
        for(int i = 0; i < keystore_size; i++){
            for(int j = 0; j < 96; j++){
                public_keys_hex_store_ns[i][j] = public_keys_hex_store[i][j];
            }
        }
}

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
void hash(uint8_t* out, uint8_t* in, size_t size){
#ifdef NRF
	    ocrypto_sha256(out, in, size);
#else // TODO:  implement hash in c
        for(int i = 0; i < 32; i++){
            out[i] = in[i];
        }
#endif
}


#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
void aes128ctr(uint8_t* key, uint8_t* iv, uint8_t* in, uint8_t* out){
    ocrypto_aes_ctr_ctx ctx;
    ocrypto_aes_ctr_init(&ctx, key, 16, iv);
    ocrypto_aes_ctr_decrypt(&ctx, out, in, 32);
}
#endif

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
int pk_in_keystore(char * public_key_hex, int offset){

        int ret = 0;

        int c = 0;

        if(keystore_size == 0){
                ret = -1;
        }

        for(int i = 0; i < keystore_size; i++){
            for(int j = 0; j < 96; j++){
                if(public_key_hex[j + offset] != public_keys_hex_store[i][j]){
                    c = 1;
                    break;
                }
            }
            if (c == 0){
                sk_sign = secret_keys_store[i];
                break;
            } else {
                if((i+1) < keystore_size){
                    c = 0;
                }else{
                    ret = -1;
                }
            }
        }
        return ret;
}

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
int ikm_sk(char* info){
        if(keystore_size >= MAX_KEYSTORE_SIZE) return -1;

        // For security, IKM MUST be infeasible to guess, e.g., generated by a trusted
        // source of randomness. IKM MUST be at least 32 bytes long, but it MAY be longer.
        unsigned char ikm[32];
#ifdef NRF
	    const int random_number_len = 144;     
        uint8_t random_number[random_number_len];
        size_t olen = random_number_len;
        int ret;

        ret = nrf_cc3xx_platform_ctr_drbg_get(NULL, random_number, random_number_len, &olen);
        
        ocrypto_sha256(ikm, random_number, random_number_len);
#else
        for(int i = 0; i < 32; i++){
            ikm[i] = rand();
        }
#endif

        //Secret key (256-bit scalar)
        blst_keygen(&sk, ikm, sizeof(ikm), info, sizeof(info));
        secret_keys_store[keystore_size] = sk;
        keystore_size++;
}

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
void sk_to_pk(blst_p1* pk){
        blst_sk_to_pk_in_g1(pk, &sk);
}

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
void sign_pk(blst_p2* sig, blst_p2* hash){
        blst_sign_pk_in_g1(sig, hash, &sk_sign);
}

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
void reset(){
        memset(secret_keys_store, 0, sizeof(secret_keys_store));
        memset(public_keys_hex_store, 0, 960);
        keystore_size = 0;
}

#ifdef NRF
__TZ_NONSECURE_ENTRY_FUNC
#endif
int import_sk(blst_scalar* sk_imp){
        int ret = 0;

        int c = 0;

        if(keystore_size == 0){
                secret_keys_store[keystore_size] = *sk_imp;
                keystore_size++;
                sk = *sk_imp;
        }else{
            for(int i = 0; i < keystore_size; i++){
                for(int x = 0; x < 32; x++){
                    if(secret_keys_store[i].b[x] != (*sk_imp).b[x]){
                        c = 1;
                        break;
                    }
            }
                if (c == 0){
                    ret = -1;
                    break;
                } else {
                    if((i+1) < keystore_size){
                        c = 0;
                    }else{
                        secret_keys_store[keystore_size] = *sk_imp;
                        keystore_size++;
                        sk = *sk_imp;
                        break;
                    }
                }
            }
        }
        return ret;
}
#endif