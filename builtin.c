define i32 @f8(i32 %v39)
{
  v11:
    %v7 = alloca i32
    store i32 %v39, ptr %v7
    %v9 = alloca i32
    %v10 = alloca i32
    store i32 0, ptr %v9
    br label %v13
  v13:
    %v15 = load i32, ptr %v9
    %v16 = load i32, ptr %v7
    %v17 = icmp slt i32 %v15, %v16
    br i1 %v17, label %v37, label %v14
  v37:
    store i32 0, ptr %v10
    br label %v18
  v18:
//if1
    %v20 = load i32, ptr %v10
    %v21 = load i32, ptr %v7
    %v22 = icmp slt i32 %v20, %v21
    br i1 %v22, label %v34, label %v19
  v34:
    %v23 = load i32, ptr %v9
    %v24 = icmp eq i32 %v23, 42
    br i1 %v24, label %v30, label %v31
  v30:
//if2
    %v25 = load i32, ptr %v9
    %v26 = icmp sgt i32 %v25, 20
    br i1 %v26, label %v28, label %v29
  v28:
    %v27 = load i32, ptr %v9
    ret i32 %v27
  v29:
//if2
    br label %v31
  v31:
    %v32 = load i32, ptr %v10
    %v33 = add i32 %v32, 1
    store i32 %v33, ptr %v10
  v19:
    %v35 = load i32, ptr %v9
    %v36 = add i32 %v35, 1
    store i32 %v36, ptr %v9
    br label %v13
  v14:
    %v38 = load i32, ptr %v9
    ret i32 %v38
  }
