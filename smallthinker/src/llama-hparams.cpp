#include "llama-hparams.h"

#include "ggml.h"

void llama_hparams::set_swa_pattern(uint32_t n_pattern) {
    for (uint32_t il = 0; il < n_layer; ++il) {
        swa_layers[il] = n_pattern == 0 || (il % n_pattern < (n_pattern - 1));
    }
}

void llama_hparams::set_dense_start_swa_pattern(uint32_t n_pattern) {
    for (uint32_t il = 0; il < n_layer; ++il) {
        swa_layers[il] = n_pattern == 0 || (il % n_pattern != 0);
    }
}

bool llama_hparams::is_swa_any() const {
    for (uint32_t il = 0; il < n_layer; ++il) {
        if (swa_layers[il]) {
            return true;
        }
    }

    return false;
}

uint32_t llama_hparams::n_head(uint32_t il) const {
    if (il < n_layer) {
        return n_head_arr[il];
    }

    GGML_ABORT("fatal error");
}

uint32_t llama_hparams::n_head_kv(uint32_t il) const {
    if (il < n_layer) {
        return n_head_kv_arr[il];
    }

    GGML_ABORT("fatal error");
}

uint32_t llama_hparams::n_ff(uint32_t il) const {
    if (il < n_layer) {
        return n_ff_arr[il];
    }

    GGML_ABORT("fatal error");
}

uint32_t llama_hparams::n_gqa(uint32_t il) const {
    const uint32_t n_head    = this->n_head(il);
    const uint32_t n_head_kv = this->n_head_kv(il);

    if (n_head_kv == 0) {
        return 0;
    }

    return n_head/n_head_kv;
}

uint32_t llama_hparams::n_embd_k_gqa(uint32_t il) const {
    const uint32_t n_head_kv = this->n_head_kv(il);

    return n_embd_head_k * n_head_kv;
}

uint32_t llama_hparams::n_embd_v_gqa(uint32_t il) const {
    const uint32_t n_head_kv = this->n_head_kv(il);

    return n_embd_head_v * n_head_kv;
}

uint32_t llama_hparams::n_embd_k_s() const {
    if (wkv_head_size != 0) {
        // for RWKV models
        return token_shift_count * n_embd;
    }

    // TODO: maybe support other convolution strides than 1
    // NOTE: since the first column of the conv_state is shifted out each time, it's not actually needed
    return (ssm_d_conv > 0 ? ssm_d_conv - 1 : 0) * ssm_d_inner;
}

uint32_t llama_hparams::n_embd_v_s() const {
    if (wkv_head_size != 0) {
        // corresponds to RWKV's wkv_states size
        return n_embd * wkv_head_size;
    }

    // corresponds to Mamba's ssm_states size
    return ssm_d_state * ssm_d_inner;
}

bool llama_hparams::is_swa(uint32_t il) const {
    if (il < n_layer) {
        return swa_layers[il];
    }

    GGML_ABORT("fatal error");
}
