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
v9:
  %v14 = alloca [3 x i32]
  %v8 = alloca [3 x i32]
  %v11 = i32 10
  %v15 = getelementptr [3 x i32], ptr %v14, i32 0, i32 0
  store i32 %v11, ptr %v15
  %v12 = i32 20
  %v16 = getelementptr [3 x i32], ptr %v14, i32 0, i32 1
  store i32 %v12, ptr %v16
  %v13 = i32 30
  %v17 = getelementptr [3 x i32], ptr %v14, i32 0, i32 2
  store i32 %v13, ptr %v17
  %v18 = getelementptr [3 x i32], ptr null, i32 1
  %v19 = ptrtoint ptr %v18 to i32
  call void @llvm.memcpy.p0.p0.i32(ptr %v8, ptr %v14, i32 %v19, i1 false)
  %v20 = i32 0
  call void exit(i32 %v20)
}
