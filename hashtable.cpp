#include <assert.h>
#include <stdlib.h>
#include "hashtable.h"


// static helps in encapsulation
// a static function can only be called within the file it is defined in.

static void h_init(HTab *htab, size_t n)
{
    assert(n>0 and ((n-1)&n)==0); // size of n should be>0 and n should be a power of two
    htab->tab=(HNode **)calloc(sizeof(HNode *),n);
    // assign space equal to an array of size n of pointer to HNode pointers 
    htab->mask=n-1;
    // mask will be n-1 because when we take mod with mask we want an index at which we
    // will try to insert the current entry.
    htab->size=0; //curr size is 0

}

static void h_insert(HTab *htab, HNode *node)
{
    size_t pos=node->hcode & htab->mask;
    // we find the position by taking mod with the mask of the hcode.
    HNode *next=htab->tab[pos]; //prepend so we don't have to traverse
    node->next=next;
    htab->size++;
}

static HNode **h_lookup(HTab *htab, HNode *key, bool(*eq)(HNode *,HNode *))
{
    if(!htab->tab) return NULL;

    size_t pos=key->hcode & htab->mask;
    HNode **from= &htab->tab[pos];// pointer to pointer of the first element at pos
    for(HNode *cur; (cur=*from)!=NULL; from=&cur->next)
    {
        if(cur->hcode==key->hcode && eq(cur,key)) return from;
        // returning 
    }
    return NULL;
}

static HNode *h_detach(HTab *htab, HNode **from)
{
    // from points to the pointer which points to the node we want to remove
    // So using from we can simply change the next value of this node so the node we want to remove
    // will be cut off.
    HNode *node=*from;
    *from=node->next;
    htab->size--;
    return node;
}

const size_t k_resizing_work=128;

static void hm_help_resizing(HMap *hmap)
{
    size_t nwork=0;
    while(nwork<k_resizing_work and hmap->ht2.size>0)
    {
        HNode **from=&hmap->ht2.tab[hmap->resizing_pos];
        if(!*from)
        {
            hmap->resizing_pos++;
            continue;
        }
        h_insert(&hmap->ht1,h_detach(&hmap->ht2,from));
        nwork++;
    }
    if(hmap->ht2.size==0 and hmap->ht2.tab)
    {
        free(hmap->ht2.tab);
        hmap->ht2=HTab{};
    }
}

static void hm_start_resizing(HMap *hmap)
{
    assert(hmap->ht2.tab==NULL);
    //creating a bigger hashtable and swapping 1 and 2
    hmap->ht2=hmap->ht1;
    h_init(&hmap->ht1,(hmap->ht1.mask+1)*2);
    hmap->resizing_pos=0;
    //this resizing pos helps to progressively resize
}

HNode *hm_lookup(HMap *hmap,HNode *key, bool(*eq)(HNode *,HNode *))
{
    hm_help_resizing(hmap);
    HNode **from=h_lookup(&hmap->ht1,key,eq);
    from=from ? from: h_lookup(&hmap->ht2,key,eq);
    return from ? *from: NULL;

    //Good use of ternary operator
    // if from is null then look in the other table too
    // if not null then do nothing.
}

const size_t k_max_load_factor=8;

void hm_insert(HMap *hmap, HNode *node)
{
    if(!hmap->ht1.tab)
    {
        h_init(&hmap->ht1,4);
    }
    h_insert(&hmap->ht1, node);
    if(!hmap->ht2.tab)
    {
        size_t load_factor=hmap->ht1.size/(hmap->ht1.mask+1);
        if(load_factor>=k_max_load_factor)
        {
            hm_start_resizing(hmap);
        }
    }
    hm_help_resizing(hmap);
}

HNode *hm_pop(HMap *hmap, HNode *key, bool (*eq)(HNode *, HNode *))
{
    hm_help_resizing(hmap);
    if(HNode **from = h_lookup(&hmap->ht2,key,eq))
    {
        return h_detach(&hmap->ht2,from);
    }
    return NULL;
}

size_t hm_size(HMap *hmap)
{
    return hmap->ht1.size+ hmap->ht2.size;
}

void hm_destroy(HMap *hmap)
{
    free(hmap->ht1.tab);
    free(hmap->ht2.tab);
    *hmap=HMap{};
}


