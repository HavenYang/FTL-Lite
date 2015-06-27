/*
U32 search_max_dirty_count_block(U32 pu)
{
    U32 i;
    struct vbt_t *vbtinfo;
    U32 vir_block = 0;
    U32 max_dirty_count = 0;

    vbtinfo = &vbt[pu];

    for (i = 0; i < vBLK_PER_PLN; i++)
    {
        if (vbtinfo->item[i].lpn_dirty_count > max_dirty_count)
        {
            vir_block = i;
            max_dirty_count = vbtinfo->item[i].lpn_dirty_count;
        }
    }

    return vir_block;
}

U32 garbage_collection(U32 pu, U32 vir_block)
{
    return SUCCESS;
}

U32 try_garbage_collection(U32 pu)
{
    U32 vir_block;

    vir_block = search_max_dirty_count_block(pu);

    return garbage_collection(pu, vir_block);
}
*/

