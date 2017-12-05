# kernel-analysis

## [PATCH] connector/cn_proc: Protect send_msg() with a local lock on RT

```
Call Trace:
dump_stack
___might_sleep
__rt_spin_lock
rt_read_lock
netlink_broadcast_filtered
netlink_broadcast
cn_netlink_send_mult
cn_netlink_send
send_msg (inlined)
proc_exit_connector
```

## cpumask: Disable CONFIG_CPUMASK_OFFSTACK for RT

```
Call Trace:
dump_stack+0x86/0xca
___might_sleep+0x14b/0x240
rt_spin_lock+0x24/0x60
get_page_from_freelist+0x83a/0x11b0
__alloc_pages_nodemask+0x15b/0x1190
alloc_pages_current+0xa1/0x1f0
new_slab+0x3e5/0x690
___slab_alloc+0x495/0x660
__slab_alloc.isra.79+0x71/0xc0
__kmalloc_node+0xe7/0x240
alloc_cpumask_var_node+0x20/0x50
alloc_cpumask_var+0xe/0x10
native_send_call_func_ipi+0x21/0x130
smp_call_function_many+0x22f/0x370
native_flush_tlb_others+0x1a4/0x3a0
flush_tlb_mm_range+0x7b/0x350
```

## Subject: x86/signal: delay calling signals on 32bit

```
Call Trace:
dump_stack+0x46/0x5c
___might_sleep+0x137/0x220
rt_spin_lock+0x1f/0x80
do_force_sig_info+0x2a/0xc0
force_sig_info+0xd/0x10
send_sigtrap+0x6f/0x80
do_debug+0x161/0x1a0
debug_stack_correct+0x2e/0x35
```
