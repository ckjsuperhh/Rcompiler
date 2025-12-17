target triple = "x86_64-unknown-linux-gnu"
declare void @llvm.memcpy.p0.p0.i32(ptr, ptr, i32, i1)
@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str.1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
define dso_local void @printInt(i32 noundef %x) local_unnamed_addr {
entry:
  %call = tail call noundef i32 (ptr, ...) @printf(ptr noundef nonnull @.str, i32 noundef %x)
  ret void
}
declare noundef i32 @printf(ptr noundef, ...) local_unnamed_addr
define dso_local void @printlnInt(i32 noundef %x) local_unnamed_addr {
entry:
  %call = tail call noundef i32 (ptr, ...) @printf(ptr noundef nonnull @.str.1, i32 noundef %x)
  ret void
}
define dso_local i32 @getInt() local_unnamed_addr {
entry:
  %addr = alloca i32
  %call = call noundef i32 (ptr, ...) @scanf(ptr noundef nonnull @.str, ptr noundef nonnull %addr)
  %result = load i32, ptr %addr
  ret i32 %result
}
declare noundef i32 @scanf(ptr noundef, ...) local_unnamed_addr
declare void @exit(i32 noundef) local_unnamed_addr
define void @main()
{
v10:
  %v8 = alloca i32
  %v9 = alloca i32
  %v12 = i32 10
  store i32 %v12, ptr %v8
  %v13 = load i32, ptr %v8
  %v14 = i32 5
  %v15 = icmp sgt i32 %v13, %v14
  br i1 %v15, label %v19, label %v20
v19:
  %v16 = load i32, ptr %v8
  %v17 = i32 5
  %v18 = sub i32 %v16, %v17
  store i32 %v18, ptr %v9
  br label %v20
v20:
  %v21 = i32 0
  call void exit(i32 %v21)
}
